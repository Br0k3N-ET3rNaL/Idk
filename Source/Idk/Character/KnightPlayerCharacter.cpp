// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/KnightPlayerCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/PlayerAbilityComponent.h"
#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/BonusEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/EffectAddingItemEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageBonusEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SingleStageEffectGroupBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/UniqueStatusEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <Components/AudioComponent.h>
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GenericPlatform/GenericPlatformMath.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Particles/ParticleSystemComponent.h>
#include <Templates/Casts.h>
#include <TimerManager.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

const FName AKnightPlayerCharacter::DamageWindow = TEXT("DamageWindow");
const FName AKnightPlayerCharacter::CanAttackAgain = TEXT("SaveAttack");
const FName AKnightPlayerCharacter::WhirlwindSocket = TEXT("Root");

AKnightPlayerCharacter::AKnightPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);

	// Initialize the sword swing audio component
	SwordSwingAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SwordSwingAudioComp"));
	SwordSwingAudioComp->SetupAttachment(MeshComp, TEXT("FX_Sword_Top"));
	SwordSwingAudioComp->PitchModulationMin = 0.8f;
	SwordSwingAudioComp->PitchModulationMax = 1.f;

	// Initialize the sword hitbox
	SwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordHitbox"));
	SwordHitbox->SetupAttachment(MeshComp, FName(TEXT("FX_Sword_Bottom")));

	SwordHitbox->SetBoxExtent(FVector(2.5, 14.5, 55.0));
	SwordHitbox->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	SwordHitbox->SetRelativeLocation(FVector(0.0, 0.0, 58.0));
	SwordHitbox->SetGenerateOverlapEvents(true);
	SwordHitbox->IgnoreActorWhenMoving(this, true);
	SwordHitbox->SetCollisionProfileName(FName(TEXT("OverlapOnlyPawn")));
	SwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &AKnightPlayerCharacter::OnSwordOverlapBegin);

	// Set the basic attack info
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom())
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();

	// Set leap's ability info
	LeapAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Leap"), FAbilityTargetingInfo::InitCircle(200.0, 1000.0, true, false), 4.0)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Leap"), TEXT("Leap to the target and deal {TargetDamage} damage to nearby enemies when landing.")))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 70.0))
		.Complete();

	// Set whirlwind's ability info
	WhirlwindAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Whirlwind"), FAbilityTargetingInfo::InitCircle(300.0, 0.0, false, false), 5.0, 5.0, 0.25)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Whirlwind"), TEXT("Unleash a flurry of blades on nearby enemies dealing {TargetDamage} damage every {Interval}s for {Duration}s.")))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 100.0))
		.Complete();

	// Set deflect's ability info
	DeflectAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Deflect"), FAbilityTargetingInfo::InitCustom(), 3.0)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Deflect"), TEXT("Apply {StatusEffect=Deflect} to self.")))
		.SetEffect(FMultiStageEffectBuilder::BeginDefault(this)
			.AddSelfEffect(FUniqueStatusEffectBuilder::BeginDefault(this)
				.SetDisplayInfo(TEXT("Deflect"), FGenericDisplayInfo(TEXT("Deflect"), TEXT("Take -50% damage from the next hit. Removed after triggering.")))
				.SetInfoPermanent()
				.AddPersistentEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
					.Init(EAttributeType::DamageTaken, 0.0, -0.5)
					.DeferCompletion())
				.AddPersistentEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
					.Init(EAddEffectLocation::OnTakeDamage, FMultiStageBonusEffectBuilder::BeginDefault(this)
						.SetSelfEffect(FBonusEffectBuilder::BeginDefault(this)
							.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
								.InitRemoveStatusEffects(TEXT("Deflect"))
								.DeferCompletion())
							.DeferCompletion())
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete();

	// Add abilities to the ability pool
	LeapIndex = AbilityPool.Add(&LeapAbilityInfo);
	WhirlwindIndex = AbilityPool.Add(&WhirlwindAbilityInfo);
	DeflectIndex = AbilityPool.Add(&DeflectAbilityInfo);

	AttributeSystemComponent->InitBaseHealthMult(1.5);
}

#if WITH_EDITOR
EDataValidationResult AKnightPlayerCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (LeapMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AKnightPlayerCharacter: Leap montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (WhirlwindMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AKnightPlayerCharacter: Whirlwind montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (WhirlwindVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AKnightPlayerCharacter: Whirlwind VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (DeflectMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AKnightPlayerCharacter: Deflect montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AKnightPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(!LeapMontage.IsNull());
	check(!WhirlwindMontage.IsNull());
	check(!WhirlwindVFX.IsNull());
	check(!DeflectMontage.IsNull());

	// Prevent audio component from playing sound on begin play
	SwordSwingAudioComp->Stop();

	// Bind ability delegates
	LeapAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AKnightPlayerCharacter::AbilityLeap);
	WhirlwindAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AKnightPlayerCharacter::AbilityWhirlwind);
	DeflectAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AKnightPlayerCharacter::AbilityDeflect);

	DisableSwordCollision();
}

void AKnightPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnLandedDelegate.ExecuteIfBound();
}

void AKnightPlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);

	if (Montage == DeflectMontage.LoadSynchronous())
	{
		AbilityComps[DeflectIndex]->EndAbility();
	}
}

void AKnightPlayerCharacter::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyBegin(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(DamageWindow))
	{
		EnableSwordCollision();

		SwordSwingAudioComp->Play();
	}
	else if (NotifyName.IsEqual(CanAttackAgain))
	{
		BasicAttackAbilityComp->EndAbility();
	}
}

void AKnightPlayerCharacter::OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyEnd(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(DamageWindow))
	{
		DisableSwordCollision();

		HitActors.Empty();
	}
}

void AKnightPlayerCharacter::BasicAttackImpl()
{
	BasicAttackAbilityComp->ApplyEffectToSelf();

	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());
}

void AKnightPlayerCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AIdkEnemyCharacter>() && !HitActors.Contains(OtherActor))
	{
		HitActors.Add(OtherActor);

		BasicAttackAbilityComp->ApplyEffectToTargetDeferred(
			CastChecked<AIdkEnemyCharacter>(OtherActor)->GetAttributeSystem());
	}
}

void AKnightPlayerCharacter::EnableSwordCollision()
{
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AKnightPlayerCharacter::DisableSwordCollision()
{
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKnightPlayerCharacter::AbilityLeap()
{
	AbilityComps[LeapIndex]->HideIndicator();

	// Prevent input while in air
	SetInputEnabled(false);

	const double Gravity = GetCharacterMovement()->GetGravityZ();
	
	// The distance to leap
	const double LeapDistance = FVector::Dist2D(GetActorLocation(), AbilityComps[LeapIndex]->GetTargetLocation());
	
	// Upward velocity needed for landing to line up with animation
	const FVector UpwardVelocity = GetActorUpVector() * ((FMath::Abs(Gravity) * LeapAirTime) / 2.0);

	// Forward velocity needed for player to travel the required distance before landing
	const FVector ForwardVelocity = GetActorForwardVector() * (LeapDistance / LeapAirTime);	

	const FVector TotalVelocity = UpwardVelocity + ForwardVelocity;

	OnLandedDelegate.BindLambda([&]()
		{
			AbilityComps[LeapIndex]->ApplyEffect();
			AbilityComps[LeapIndex]->EndAbility();
			SetInputEnabled(true);

			OnLandedDelegate.Unbind();
		});

	AnimInstance->Montage_Play(LeapMontage.LoadSynchronous());

	LaunchCharacter(TotalVelocity, true, true);
}

void AKnightPlayerCharacter::AbilityWhirlwind()
{
	AbilityComps[WhirlwindIndex]->HideIndicator();

	AnimInstance->Montage_Play(WhirlwindMontage.LoadSynchronous());

	FTimerDelegate ApplyEffectDelegate 
		= FTimerDelegate::CreateUObject(AbilityComps[WhirlwindIndex], &UPlayerAbilityComponent::ApplyEffectDeferred);

	GetWorldTimerManager().SetTimer(WhirlwindApplyEffectTimer, ApplyEffectDelegate, WhirlwindAbilityInfo.GetHitRate(), true, WhirlwindActivationDelay);

	FTimerDelegate OnAbilityEndDelegate
		= FTimerDelegate::CreateUObject(this, &AKnightPlayerCharacter::OnWhirlwindEnd);

	GetWorldTimerManager().SetTimer(WhirlwindDurationTimer, OnAbilityEndDelegate, WhirlwindAbilityInfo.GetDuration(), false);

	AbilityComps[WhirlwindIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale
		, WhirlwindSocket);

	AbilityComps[WhirlwindIndex]->ApplyEffectToSelf();

	// Display whirlwind's VFX
	UParticleSystemComponent* ParticleSystem =
		UGameplayStatics::SpawnEmitterAttached(WhirlwindVFX.LoadSynchronous(), GetMesh(), WhirlwindSocket);
}

void AKnightPlayerCharacter::OnWhirlwindEnd()
{
	GetWorldTimerManager().ClearTimer(WhirlwindApplyEffectTimer);

	AbilityComps[WhirlwindIndex]->EndAbility();
}

void AKnightPlayerCharacter::AbilityDeflect()
{
	AnimInstance->Montage_Play(DeflectMontage.LoadSynchronous());

	AbilityComps[DeflectIndex]->ApplyEffect();
}
