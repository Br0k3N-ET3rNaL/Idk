// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/IdkEnemyCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AIAbilityComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/IdkCharacterMovementComponent.h"
#include "Idk/Character/IdkCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/OrientToPlayerComponent.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/UI/HealthAndStatusBarWidget.h"
#include "Idk/UI/OverheadWidgetComponent.h"
#include <AIController.h>
#include <Animation/AnimMontage.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BlackboardData.h>
#include <Blueprint/UserWidget.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Containers/Array.h>
#include <CoreGlobals.h>
#include <Curves/RealCurve.h>
#include <Engine/CollisionProfile.h>
#include <Engine/CurveTable.h>
#include <Engine/EngineTypes.h>
#include <Engine/TimerHandle.h>
#include <GameFramework/Actor.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Logging/LogMacros.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <PhysicsEngine/PhysicalAnimationComponent.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

const TArray<FName> AIdkEnemyCharacter::AbilityCooldownBBKeys = { TEXT("IsBasicAttackOnCooldown"), TEXT("IsAbility1OnCooldown"), TEXT("IsAbility2OnCooldown"), TEXT("IsAbility3OnCooldown") };
const TArray<FName> AIdkEnemyCharacter::AbilityBBKeys = { TEXT("IsUsingBasicAttack"), TEXT("IsUsingAbility1"), TEXT("IsUsingAbility2"), TEXT("IsUsingAbility3") };
const FName AIdkEnemyCharacter::StunnedKey = TEXT("Stunned");
const FName AIdkEnemyCharacter::IsUsingAnyAbilityKey = TEXT("IsUsingAnyAbility");
const FName AIdkEnemyCharacter::RagdollCollisionProfile = TEXT("Ragdoll");
const FName AIdkEnemyCharacter::ExperienceCurve = TEXT("Experience");
const FName AIdkEnemyCharacter::GoldCurve = TEXT("Gold");

AIdkEnemyCharacter::AIdkEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass(ACharacter::CharacterMovementComponentName, UIdkCharacterMovementComponent::StaticClass()))
{
	AttributeSystemComponent->SetStunnedDelegate.BindUObject(this, &AIdkEnemyCharacter::SetStunned);

	// Initialize the basic attack ability component
	UAIAbilityComponent* BasicAttackAbilityComp = CreateDefaultSubobject<UAIAbilityComponent>(TEXT("BasicAttackAbilityComp"));
	BasicAttackAbilityComp->SetupAttachment(RootComponent);
	BasicAttackAbilityComp->SetAttributeSystem(AttributeSystemComponent);
	AbilityComps.Insert(BasicAttackAbilityComp, 0);

	OrientToPlayerComp = CreateDefaultSubobject<UOrientToPlayerComponent>(TEXT("OrientToPlayerComp"));
	OrientToPlayerComp->SetupAttachment(GetCapsuleComponent());

	// Initialize the overhead health and status bar display
	HealthAndStatusBarWidgetComp = CreateDefaultSubobject<UOverheadWidgetComponent>(TEXT("HealthAndStatusBarWidgetComp"));
	HealthAndStatusBarWidgetComp->SetupAttachment(OrientToPlayerComp);
	HealthAndStatusBarWidgetComp->SetRelativeLocation(FVector(0.0, 0.0, 60.0));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationYaw = false;

	// Initialize the movement component
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bUseControllerDesiredRotation = true;
	MoveComp->bUseRVOAvoidance = true;

	AttributeSystemComponent->InitBaseMoveSpeed(400.0);
}

#if WITH_EDITOR
EDataValidationResult AIdkEnemyCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (CurveTable.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AIdkEnemyCharacter: Curve table is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AIdkEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UIdkGameInstance* GameInstance = UIdkGameInstance::GetGameInstance(GetWorld());

	check(GameInstance);

	TSubclassOf<UHealthAndStatusBarWidget> HealthAndStatusBarWidgetClass = GameInstance->GetHealthAndStatusBarWidgetClass();

	UHealthAndStatusBarWidget* HealthAndStatusBarWidget = CreateWidget<UHealthAndStatusBarWidget>(GetWorld()->GetFirstPlayerController(), HealthAndStatusBarWidgetClass);

	check(HealthAndStatusBarWidget);

	SetHealthAndStatusBarWidget(*HealthAndStatusBarWidget);

	Player = CastChecked<AIdkPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	check(!CurveTable.IsNull());

	const UCurveTable* Curves = CurveTable.LoadSynchronous();
	check(Curves->FindCurveUnchecked(ExperienceCurve));
	check(Curves->FindCurveUnchecked(GoldCurve));

	AAIController* AIController = CastChecked<AAIController>(GetController());
	Blackboard = AIController->GetBlackboardComponent();
	check(Blackboard);

	UBlackboardData* BlackboardData = Blackboard->GetBlackboardAsset();

	BindAbilities();

	// Set up the ability delegates for the basic attack
	BasicAttackAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AIdkEnemyCharacter::BasicAttack);
	AbilityComps[BasicAttackAbilityIndex]->SetAbility(BasicAttackAbilityInfo);
	AbilityComps[BasicAttackAbilityIndex]->OnAbilityEndDelegate.BindUObject(this, &AIdkEnemyCharacter::OnAbilityEnd, BasicAttackAbilityIndex);
	AbilityComps[BasicAttackAbilityIndex]->SetOnCooldownDelegate.BindUObject(this, &AIdkEnemyCharacter::SetAbilityCooldownBBKey, BasicAttackAbilityIndex);
	Blackboard->SetValueAsBool(AbilityCooldownBBKeys[BasicAttackAbilityIndex], false);

	check(AbilityPool.Num() <= 3);

	// Create and set up ability components for each ability in the enemy's ability pool
	for (int32 i = 1; i <= AbilityPool.Num(); ++i)
	{
		check(AbilityPool[i - 1]);

		UAIAbilityComponent* AbilityComp =
			CastChecked<UAIAbilityComponent>(AddComponentByClass(UAIAbilityComponent::StaticClass(), true, {}, false));
		AbilityComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		AddInstanceComponent(AbilityComp);

		AbilityComp->SetAttributeSystem(AttributeSystemComponent);
		AbilityComp->SetAbility(*AbilityPool[i - 1]);
		AbilityComp->OnAbilityEndDelegate.BindUObject(this, &AIdkEnemyCharacter::OnAbilityEnd, i);
		AbilityComp->SetOnCooldownDelegate.BindUObject(this, &AIdkEnemyCharacter::SetAbilityCooldownBBKey, i);
		AbilityComps.Add(AbilityComp);

		const FName& KeyName = AbilityCooldownBBKeys[i];
		Blackboard->SetValueAsBool(KeyName, false);
	}

	PostAbilityComponentsCreated();
}

void AIdkEnemyCharacter::PostActorCreated()
{
	if (UIdkGameInstance* GameInstance = UIdkGameInstance::GetGameInstance(GetWorld()))
	{
		// Display spawn animation when spawned
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld()
			, &GameInstance->GetSpawnVFX()
			, GetActorTransform());
	}

	Super::PostActorCreated();
}

void AIdkEnemyCharacter::SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget)
{
	Super::SetHealthAndStatusBarWidget(HealthAndStatusBarWidget);

	HealthAndStatusBarWidgetComp->SetWidget(&HealthAndStatusBarWidget);
}

void AIdkEnemyCharacter::Init(const uint8 InLevel)
{
	Level = InLevel;

	AttributeSystemComponent->InitHealth();
}

void AIdkEnemyCharacter::SetStunned(bool bStunned) const
{
	Blackboard->SetValueAsBool(StunnedKey, bStunned);
}

void AIdkEnemyCharacter::ActivateAbility(const AActor* Target, const int32 Index)
{
	Blackboard->SetValueAsBool(IsUsingAnyAbilityKey, true);
	Blackboard->SetValueAsBool(AbilityBBKeys[Index], true);

	AbilityComps.RangeCheck(Index);

	AbilityComps[Index]->SetTarget(Target);
	AbilityComps[Index]->StartAbility();
}

void AIdkEnemyCharacter::OnAbilityEnd(const int32 Index)
{
	Blackboard->SetValueAsBool(AbilityBBKeys[Index], false);
	Blackboard->SetValueAsBool(IsUsingAnyAbilityKey, false);
}

void AIdkEnemyCharacter::SetAbilityCooldownBBKey(const bool bOnCooldown, const int32 Index)
{
	Blackboard->SetValueAsBool(AbilityCooldownBBKeys[Index], bOnCooldown);
}

FVector AIdkEnemyCharacter::GetPosInAbilityRange(const AActor* Target, const int32 Index) const
{
	AbilityComps.RangeCheck(Index);

	return AbilityComps[Index]->GetPosInRangeOfTarget(Target);
}

bool AIdkEnemyCharacter::IsInAbilityRange(const AActor* Target, const int32 Index) const
{
	AbilityComps.RangeCheck(Index);

	return AbilityComps[Index]->IsInRangeOfTarget(Target);
}

bool AIdkEnemyCharacter::AreAbilityBonusConditionsFulfilled(const int32 Index) const
{
	AbilityComps.RangeCheck(Index);

	return AreAbilityBonusConditionsFulfilledImpl(Index);
}

uint8 AIdkEnemyCharacter::GetLevel() const
{
	return Level;
}

void AIdkEnemyCharacter::OnDeath()
{
	Super::OnDeath();

	// Disable collisions
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	for (UAIAbilityComponent* AbilityComp : AbilityComps)
	{
		AbilityComp->DestroyComponent();
	}

	GetController()->UnPossess();
	HealthAndStatusBarWidgetComp->SetVisibility(false);

	if (OnDeathDelegate.IsBound())
	{
		const UCurveTable* Curves = CurveTable.LoadSynchronous();
		const float Experience = Curves->FindCurveUnchecked(ExperienceCurve)->Eval(Level);
		const float Gold = Curves->FindCurveUnchecked(GoldCurve)->Eval(Level);
		OnDeathDelegate.Execute(Experience, Gold);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDeathDelegate not bound"));
	}

	AnimInstance->Montage_Play(DeathAnimation.LoadSynchronous());
}

void AIdkEnemyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);

	if (Montage == DeathAnimation.LoadSynchronous() && bRagdollOnDeath)
	{
		GetWorldTimerManager().ClearTimer(DeathBlendTimer);

		// Stop blending death animation with ragdoll
		GetMesh()->SetAllBodiesPhysicsBlendWeight(1.f);
		PhysicalAnimationComp->SetStrengthMultiplyer(0.f);

		FTimerHandle Timer;

		GetWorldTimerManager().SetTimer(Timer, [this]()
			{
				AnimInstance->FreezeRagdollPose();
				PhysicalAnimationComp->SetStrengthMultiplyer(0.1f);

				// Disabling ragdoll immediately can cause a sudden jerk, so it's disabled on the next tick
				GetWorldTimerManager().SetTimerForNextTick([this]()
					{
						DisableRagdoll();
					});
				
			}, RagdollDuration, false);
	}
}

void AIdkEnemyCharacter::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DeathAnimation.LoadSynchronous() && bRagdollOnDeath)
	{
		DeathBlendDuration = DeathAnimation.LoadSynchronous()->GetDefaultBlendOutTime();
		DeathBlendDurationRemaining = DeathBlendDuration;

		EnableRagdoll();

		// Start blending death animation with ragdoll
		GetWorldTimerManager().SetTimer(DeathBlendTimer
			, FTimerDelegate::CreateUObject(this, &AIdkEnemyCharacter::UpdateDeathBlend)
			, DeathBlendInterval, true);
	}
}

void AIdkEnemyCharacter::BindAbilities()
{
}

void AIdkEnemyCharacter::PostAbilityComponentsCreated()
{
}

bool AIdkEnemyCharacter::AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const
{
	return true;
}

void AIdkEnemyCharacter::EnableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();

	MeshComp->SetCollisionProfileName(RagdollCollisionProfile);

	PhysicalAnimationComp = NewObject<UPhysicalAnimationComponent>(this);
	PhysicalAnimationComp->RegisterComponent();
	PhysicalAnimationComp->SetSkeletalMeshComponent(MeshComp);
	PhysicalAnimationComp->ApplyPhysicalAnimationProfileBelow(NAME_None, TEXT("Default"));
	PhysicalAnimationComp->SetStrengthMultiplyer(0.f);

	MeshComp->SetAllBodiesPhysicsBlendWeight(0.1f);
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
}

void AIdkEnemyCharacter::DisableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();

	PhysicalAnimationComp->SetStrengthMultiplyer(0.f);

	MeshComp->SetAllBodiesPhysicsBlendWeight(0.f);
	MeshComp->SetComponentTickEnabled(false);
	MeshComp->SetAllBodiesSimulatePhysics(false);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->PutAllRigidBodiesToSleep();
}

void AIdkEnemyCharacter::UpdateDeathBlend()
{
	DeathBlendDurationRemaining -= DeathBlendInterval;

	PhysicalAnimationComp->SetStrengthMultiplyer(1.f - (DeathBlendDurationRemaining / DeathBlendDuration));
}
