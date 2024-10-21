// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Rampage.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AIAbilityComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/UniqueStatusEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include "Idk/GameSystems/Projectile.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Engine/CollisionProfile.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

ARampage::ARampage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);

	// Initialize the right hand hitbox
	RightHandHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandHitbox"));
	RightHandHitbox->SetupAttachment(MeshComp, RightHandHitboxSocket);

	RightHandHitbox->SetGenerateOverlapEvents(true);
	RightHandHitbox->IgnoreActorWhenMoving(this, true);
	RightHandHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RightHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &ARampage::OnHandOverlapBegin);

	// Initialize the left hand hitbox
	LeftHandHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandHitbox"));
	LeftHandHitbox->SetupAttachment(MeshComp, LeftHandHitboxSocket);

	LeftHandHitbox->SetGenerateOverlapEvents(true);
	LeftHandHitbox->IgnoreActorWhenMoving(this, true);
	LeftHandHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LeftHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &ARampage::OnHandOverlapBegin);

	// Initialize the rock static mesh component
	RockMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockMeshComp"));
	RockMeshComp->SetupAttachment(GetMesh(), RockAttachSocket);
	RockMeshComp->SetVisibility(false);

	// Set the basic attack info
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom(90.0))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();

	// Set smash's ability info
	SmashAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Smash"), FAbilityTargetingInfo::InitCircle(200.0, 30.0, false, false), 5.0)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 20.0))
		.Complete();

	// Set rip n' toss's ability info
	RipNTossAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("RipNToss"), FAbilityTargetingInfo::InitCustom(-1.0, true), 5.0)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 20.0))
		.Complete();
	
	// Set enrage's ability info
	EnrageAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Enrage"), FAbilityTargetingInfo::InitCustom(), -1.0)
		.SetEffect(FMultiStageEffectBuilder::BeginDefault(this)
			.AddSelfEffect(FUniqueStatusEffectBuilder::BeginDefault(this)
				.SetDisplayInfo(TEXT("Enrage"), FGenericDisplayInfo(TEXT("Enrage"), TEXT("Enrages when below 50% health, gaining +100% damage.")))
				.SetInfoPermanent()
				.AddPersistentEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
					.Init(EAttributeType::Damage, 0.0, 1.0)
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete();

	// Add abilities to the ability pool
	SmashIndex = AbilityPool.Add(&SmashAbilityInfo) + 1;
	RipNTossIndex = AbilityPool.Add(&RipNTossAbilityInfo) + 1;
	EnrageIndex = AbilityPool.Add(&EnrageAbilityInfo) + 1;

	AttributeSystemComponent->InitBaseHealthMult(3.0);
}

#if WITH_EDITOR
EDataValidationResult ARampage::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (SmashMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARampage: Smash montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (RipMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARampage: Rip montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (TossMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARampage: Toss montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (RipNTossProjectileClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARampage: RipNToss projectile class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (EnrageMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARampage: Enrage montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ARampage::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyBegin(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(RightDamageWindow))
	{
		RightHandHitbox->SetCollisionProfileName(HandCollisionProfile);
	}
	else if (NotifyName.IsEqual(LeftDamageWindow))
	{
		LeftHandHitbox->SetCollisionProfileName(HandCollisionProfile);
	}
	else if (NotifyName.IsEqual(CanAttackAgain))
	{
		AbilityComps[BasicAttackAbilityIndex]->EndAbility();
	}
	else if (NotifyName.IsEqual(SmashImpactNotify))
	{
		OnSmashImpact();
	}
	else if (NotifyName.IsEqual(TossReleaseNotify))
	{
		Toss();
	}
}

void ARampage::OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyEnd(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(RightDamageWindow))
	{
		RightHandHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

		HitActors.Empty();
	}
	else if (NotifyName.IsEqual(LeftDamageWindow))
	{
		LeftHandHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

		HitActors.Empty();
	}
}

void ARampage::BasicAttack()
{
	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());
}

void ARampage::OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AIdkPlayerCharacter>() && !HitActors.Contains(OtherActor))
	{
		HitActors.Add(OtherActor);

		AbilityComps[BasicAttackAbilityIndex]->ApplyEffectToTargetDeferred(
			CastChecked<AIdkPlayerCharacter>(OtherActor)->GetAttributeSystem());
	}
}

void ARampage::BindAbilities()
{
	SmashAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &ARampage::AbilitySmash);
	RipNTossAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &ARampage::AbilityRipNToss);
	EnrageAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &ARampage::AbilityEnrage);
}

void ARampage::PostAbilityComponentsCreated()
{
	AbilityComps[SmashIndex]->AttachToComponent(
		GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SmashCollisionSocket);
	AbilityComps[SmashIndex]->SetUsingAbsoluteRotation(true);
}

bool ARampage::AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const
{
	// Enrage can only be activated if health is below the activation threshold
	if (Index == EnrageIndex)
	{
		return !bEnraged && AttributeSystemComponent->GetHealthPercent() <= EnrageHealthThreshold;
	}

	return Super::AreAbilityBonusConditionsFulfilledImpl(Index);
}

void ARampage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BasicAttackChain.GetCurrentAttack())
	{
		AbilityComps[BasicAttackAbilityIndex]->EndAbility();
	}
	else if (Montage == SmashMontage.LoadSynchronous())
	{
		AbilityComps[SmashIndex]->EndAbility();
	}
	else if (Montage == RipMontage.LoadSynchronous())
	{
		FTimerHandle DelayTossTimer;

		GetWorldTimerManager().SetTimer(DelayTossTimer, this, &ARampage::BeginToss, TossDelay, false);
	}
	else if (Montage == TossMontage.LoadSynchronous())
	{
		AbilityComps[RipNTossIndex]->EndAbility();
	}
	else if (Montage == EnrageMontage.LoadSynchronous())
	{
		ActivateEnrage();
	}

	Super::OnMontageEnded(Montage, bInterrupted);
}

void ARampage::AbilitySmash()
{
	AbilityComps[SmashIndex]->ShowIndicator();

	AnimInstance->Montage_Play(SmashMontage.LoadSynchronous());
}

void ARampage::OnSmashImpact()
{
	AbilityComps[SmashIndex]->HideIndicator();
	AbilityComps[SmashIndex]->ApplyEffect();
}

void ARampage::AbilityRipNToss()
{
	RockMeshComp->SetVisibility(true);

	AnimInstance->Montage_Play(RipMontage.LoadSynchronous());
}

void ARampage::BeginToss()
{
	AnimInstance->Montage_Play(TossMontage.LoadSynchronous());
}

void ARampage::Toss()
{
	AbilityComps[RipNTossIndex]->ApplyEffectToSelf();

	const FTransform SocketTransform = GetMesh()->GetSocketTransform(RockAttachSocket);
	const FVector ProjectileLocation = SocketTransform.GetLocation();
	FVector ProjectileOrientation = AbilityComps[RipNTossIndex]->GetTargetLocation() - ProjectileLocation;
	ProjectileOrientation.Normalize();

	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
		RipNTossProjectileClass.LoadSynchronous(), SocketTransform);

	Projectile->OnCollisionDelegate.BindUObject(AbilityComps[RipNTossIndex], &UAIAbilityComponent::ApplyEffectToTargetDeferred);
	Projectile->Init(ProjectileOrientation, AIdkPlayerCharacter::StaticClass(), this);

	// Projectile shares the same mesh so hide this one
	RockMeshComp->SetVisibility(false);

	UGameplayStatics::FinishSpawningActor(Projectile, SocketTransform);
}

void ARampage::AbilityEnrage()
{
	AnimInstance->Montage_Play(EnrageMontage.LoadSynchronous());
}

void ARampage::ActivateEnrage()
{
	bEnraged = true;

	AbilityComps[EnrageIndex]->ApplyEffectToSelf();
	AbilityComps[EnrageIndex]->EndAbility();
}
