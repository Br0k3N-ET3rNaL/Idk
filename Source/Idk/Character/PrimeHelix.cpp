// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/PrimeHelix.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AIAbilityComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include "Idk/GameSystems/Projectile.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <CharacterMovementComponentAsync.h>
#include <CollisionQueryParams.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Misc/DataValidation.h>
#include <Particles/ParticleSystemComponent.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

APrimeHelix::APrimeHelix(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(250.f, 250.f);

	// Prime Helix doesn't have a proper physics asset for ragdolling
	bRagdollOnDeath = false;

	// Set the basic attack info
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom(1000.0, true, true))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();

	// Set barrage's ability info
	BarrageAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Barrage"), FAbilityTargetingInfo::InitCustom(-1.0, true, true), 5.0)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 5.0))
		.Complete();

	// Set shockwave's ability info
	ShockwaveAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Shockwave"), FAbilityTargetingInfo::InitCircle(700.0, 0.0, false, false), 10.0)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 50.0))
		.Complete();

	// Set slam's ability info
	SlamAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Slam"), FAbilityTargetingInfo::InitRectangle(200.0, 1000.0, 0.0, false, false), 3.0)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 20.0))
		.Complete();

	// Add abilities to the ability pool
	BarrageIndex = AbilityPool.Add(&BarrageAbilityInfo) + 1;
	ShockwaveIndex = AbilityPool.Add(&ShockwaveAbilityInfo) + 1;
	SlamIndex = AbilityPool.Add(&SlamAbilityInfo) + 1;

	AttributeSystemComponent->InitBaseHealthMult(2.0);
}

#if WITH_EDITOR
EDataValidationResult APrimeHelix::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (BasicAttackVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Basic attack VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BarrageMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Barrage montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BarrageProjectileClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Barrage projectile class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (ShockwaveMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Shockwave montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (SlamMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Slam montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (SlamGroundVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("APrimeHelix: Slam ground VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void APrimeHelix::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyBegin(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(BasicAttackChargeEndNotify))
	{
		OnBasicAttackChargeEnd();
	}
	else if (NotifyName.IsEqual(ShockwaveChargeEndNotify))
	{
		OnShockwaveChargeEnd();
	}
	else if (NotifyName.IsEqual(BarrageFireNotify))
	{
		OnBarrageChargeEnd();
	}
	else if (NotifyName.IsEqual(SlamImpactNotify))
	{
		OnSlamChargeEnd();
	}
}

void APrimeHelix::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == SlamMontage.LoadSynchronous())
	{
		AbilityComps[SlamIndex]->EndAbility();
	}

	Super::OnMontageEnded(Montage, bInterrupted);
}

void APrimeHelix::BindAbilities()
{
	BarrageAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &APrimeHelix::AbilityBarrage);
	ShockwaveAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &APrimeHelix::AbilityShockwave);
	SlamAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &APrimeHelix::AbilitySlam);
}

void APrimeHelix::PostAbilityComponentsCreated()
{
	AbilityComps[BarrageIndex]->AttachToComponent(
		GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BarrageOriginSocket);

	AbilityComps[ShockwaveIndex]->AttachToComponent(
		GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShockwaveCollisionSocket);
	AbilityComps[ShockwaveIndex]->SetUsingAbsoluteRotation(true);

	AbilityComps[SlamIndex]->AttachToComponent(
		GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SlamCollisionSocket);
}

void APrimeHelix::BasicAttack()
{
	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());
}

bool APrimeHelix::AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const
{
	if (Index == BarrageIndex)
	{
		return AttributeSystemComponent->GetHealthPercent() <= BarrageHealthThreshold;
	}

	return Super::AreAbilityBonusConditionsFulfilledImpl(Index);
}

void APrimeHelix::OnBasicAttackChargeEnd()
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	const FVector BasicAttackOrigin = MeshComp->GetSocketTransform(BasicAttackOriginSocket).GetLocation();

	FVector TargetLocation = AbilityComps[BasicAttackAbilityIndex]->GetInitialTargetLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, BasicAttackOrigin, TargetLocation,
		ECollisionChannel::ECC_Pawn, QueryParams);

	AbilityComps[BasicAttackAbilityIndex]->ApplyEffect();

	if (AActor* Actor = HitResult.GetActor())
	{
		TargetLocation = Actor->GetActorLocation();

		if (Actor->IsA<AIdkPlayerCharacter>())
		{
			AbilityComps[BasicAttackAbilityIndex]->ApplyEffectToTargetDeferred(
				CastChecked<AIdkPlayerCharacter>(Actor)->GetAttributeSystem());
		}
	}

	const FRotator VFXOrientation = (TargetLocation - BasicAttackOrigin).ToOrientationRotator();

	UParticleSystemComponent* VFX = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(), BasicAttackVFX.LoadSynchronous(), BasicAttackOrigin, VFXOrientation);

	VFX->OnSystemFinished.AddDynamic(this, &APrimeHelix::OnBasicAttackFireEnd);
}

void APrimeHelix::OnBasicAttackFireEnd(UParticleSystemComponent* PSystem)
{
	AbilityComps[BasicAttackAbilityIndex]->EndAbility();
}

void APrimeHelix::AbilityBarrage()
{
	AnimInstance->Montage_Play(BarrageMontage.LoadSynchronous());
}

void APrimeHelix::OnBarrageChargeEnd()
{
	UWorld* World = GetWorld();

	FVector StartingOrientation = GetMesh()->GetSocketQuaternion(BarrageOriginSocket).Vector();
	StartingOrientation = StartingOrientation.RotateAngleAxis(-BarrageLaunchAngle, GetActorRightVector());
	StartingOrientation = StartingOrientation.RotateAngleAxis(-BarrageSpreadHalfAngle, GetActorUpVector());

	// Angle between each projectile
	const double Spread = BarrageSpreadHalfAngle / ((BarrageNumProjectiles - 1) / 2.0);

	FTransform ProjectileTransform = FTransform(GetMesh()->GetSocketLocation(BarrageOriginSocket));

	for (uint8 i = 0; i < BarrageNumProjectiles; ++i)
	{
		ProjectileTransform.SetRotation(StartingOrientation.ToOrientationQuat());

		AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(
			BarrageProjectileClass.LoadSynchronous(), ProjectileTransform);

		Projectile->OnCollisionDelegate.BindUObject(
			AbilityComps[BarrageIndex], &UAIAbilityComponent::ApplyEffectToTargetDeferred);
		Projectile->InitHoming(StartingOrientation, AIdkPlayerCharacter::StaticClass(), this, Player);

		UGameplayStatics::FinishSpawningActor(Projectile, ProjectileTransform);

		StartingOrientation = StartingOrientation.RotateAngleAxis(Spread, GetActorUpVector());
	}

	AbilityComps[BarrageIndex]->EndAbility();
}

void APrimeHelix::AbilityShockwave()
{
	AbilityComps[ShockwaveIndex]->ShowIndicator();

	AnimInstance->Montage_Play(ShockwaveMontage.LoadSynchronous());
}

void APrimeHelix::OnShockwaveChargeEnd()
{
	AbilityComps[ShockwaveIndex]->HideIndicator();
	AbilityComps[ShockwaveIndex]->ApplyEffect();
	AbilityComps[ShockwaveIndex]->EndAbility();
}

void APrimeHelix::AbilitySlam()
{
	AbilityComps[SlamIndex]->ShowIndicator();
	AnimInstance->Montage_Play(SlamMontage.LoadSynchronous());
}

void APrimeHelix::OnSlamChargeEnd()
{
	AbilityComps[SlamIndex]->HideIndicator();
	FFindFloorResult FloorResult;
	GetCharacterMovement()->FindFloor(GetCapsuleComponent()->GetComponentLocation(), FloorResult, true);

	const double FloorZ = FloorResult.HitResult.ImpactPoint.Z;

	const FVector2D SocketPos = FVector2D(GetMesh()->GetSocketLocation(SlamCollisionSocket));

	// Offset to the center of the area of effect
	const FVector2D FXOffset = FVector2D(GetOwner()->GetActorForwardVector() * (SlamAbilityInfo.GetTargetingInfo().GetLength() / 2.0));
	
	const FVector FXPos = FVector(SocketPos + FXOffset, FloorZ);
	const FQuat FXRot = GetActorQuat();
	const FTransform FXTransform = FTransform(FXRot, FXPos);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SlamGroundVFX.LoadSynchronous(), FXTransform);

	AbilityComps[SlamIndex]->ApplyEffect();
}
