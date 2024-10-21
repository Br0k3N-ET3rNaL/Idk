// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/MagePlayerCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/PlayerAbilityComponent.h"
#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/UniqueStatusEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include "Idk/GameSystems/Projectile.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <Delegates/Delegate.h>
#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Particles/ParticleSystemComponent.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <TimerManager.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

const FName AMagePlayerCharacter::BasicAttackSpawnSocket = TEXT("Muzzle_01");
const FName AMagePlayerCharacter::BasicAttackSpawnNotify = TEXT("BasicAttackSpawnProjectile");
const FName AMagePlayerCharacter::CanAttackAgain = TEXT("SaveAttack");
const FName AMagePlayerCharacter::SpawnBlackholeNotify = TEXT("SpawnBlackhole");
const FName AMagePlayerCharacter::BurdenSpawnNotify = TEXT("BurdenSpawnProjectile");

AMagePlayerCharacter::AMagePlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set the basic attack info
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom(1000.0, true, true), 0.25)
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();

	// Set meteor's ability info
	MeteorAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Meteor"), FAbilityTargetingInfo::InitCircle(350.0, 1000.0, true, true), 5.0)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Meteor"), TEXT("Summon a meteor at the target location, dealing {TargetDamage} damage to nearby enemies on impact.")))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 90.0))
		.Complete();

	// Set blackhole's ability info
	BlackholeAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Blackhole"), FAbilityTargetingInfo::InitCircle(500.0, 1000.0, true, true), 4.0, 5.25, 0.25)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Blackhole"), TEXT("Summon a blackhole at the target location, sucking in nearby enemies and dealing {TargetDamage} damage every {Interval}s for {Duration}s.")))
		.SetEffect(FMultiStageEffectBuilder::BeginDefault(this)
			.AddTargetEffect(FSimpleEffectBuilder::BeginDefault(this)
				.InitPush(-2.0)
				.DeferCompletion())
			.AddTargetEffect(FSimpleEffectBuilder::BeginDefault(this)
				.InitDamage(105.0)
				.DeferCompletion())
			.DeferCompletion())
		.Complete();

	// Set burden's ability info
	BurdenAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("Burden"), FAbilityTargetingInfo::InitCustom(1000.0, true, true), 2.0)
		.SetDisplayInfo(FGenericDisplayInfo(TEXT("Burden"), TEXT("Fire a projectile dealing {TargetDamage} damage and applying {StatusEffect=Burdened} on impact.")))
		.SetEffect(FMultiStageEffectBuilder::BeginDefault(this)
			.AddTargetEffect(FSimpleEffectBuilder::BeginDefault(this)
				.InitDamage(40.0)
				.DeferCompletion())
			.AddTargetEffect(FUniqueStatusEffectBuilder::BeginDefault(this)
				.SetDisplayInfo(TEXT("Burdened"), FGenericDisplayInfo(TEXT("Burdened"), TEXT("Movement speed reduced by 20%.")))
				.SetInfo(5.0, 3, true)
				.AddPersistentEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
					.Init(EAttributeType::MovementSpeed, 0.0, -0.2)
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete();

	// Add abilities to the ability pool
	MeteorIndex = AbilityPool.Add(&MeteorAbilityInfo);
	BlackholeIndex = AbilityPool.Add(&BlackholeAbilityInfo);
	BurdenIndex = AbilityPool.Add(&BurdenAbilityInfo);

	AttributeSystemComponent->InitBaseHealthMult(1.1);
}

#if WITH_EDITOR
EDataValidationResult AMagePlayerCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (BasicAttackProjectileClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Basic attack projectile class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (MeteorMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Meteor montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (MeteorVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Meteor VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BlackholeMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Blackhole montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BlackholeVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Blackhole VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BurdenMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Burden montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (BurdenProjectileClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AMagePlayerCharacter: Burden projectile class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AMagePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(!BasicAttackProjectileClass.IsNull());
	check(!MeteorMontage.IsNull());
	check(!MeteorVFX.IsNull());
	check(!BlackholeMontage.IsNull());
	check(!BlackholeVFX.IsNull());
	check(!BurdenMontage.IsNull());
	check(!BurdenProjectileClass.IsNull());

	// Bind ability delegates
	MeteorAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AMagePlayerCharacter::AbilityMeteor);
	BlackholeAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AMagePlayerCharacter::AbilityBlackhole);
	BurdenAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AMagePlayerCharacter::AbilityBurden);
}

void AMagePlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);

	if (Montage == BurdenMontage.LoadSynchronous())
	{
		OnBurdenComplete();
	}
}

void AMagePlayerCharacter::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyBegin(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(BasicAttackSpawnNotify))
	{
		SpawnBasicAttackProjectile();
	}
	else if (NotifyName.IsEqual(CanAttackAgain))
	{
		BasicAttackAbilityComp->EndAbility();
	}
	else if (NotifyName.IsEqual(SpawnBlackholeNotify))
	{
		SpawnBlackhole();
	}
	else if (NotifyName.IsEqual(BurdenSpawnNotify))
	{
		SpawnBurdenProjectile();
	}
}

void AMagePlayerCharacter::BasicAttackImpl()
{
	BasicAttackAbilityComp->ApplyEffectToSelf();

	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());
}

void AMagePlayerCharacter::SpawnBasicAttackProjectile()
{
	const FVector ProjectilePos = GetMesh()->GetSocketLocation(BasicAttackSpawnSocket);
	const FRotator ProjectileRot = GetActorRotation();

	// Starting projectile transform
	const FTransform ProjectileTransform = FTransform(ProjectileRot, ProjectilePos);

	AProjectile* Projectile = 
		GetWorld()->SpawnActorDeferred<AProjectile>(BasicAttackProjectileClass.LoadSynchronous(), ProjectileTransform);

	Projectile->Init(ProjectileTransform.GetRotation().Vector(), AIdkEnemyCharacter::StaticClass(), this);
	Projectile->OnCollisionDelegate.BindUObject(
		BasicAttackAbilityComp, &UPlayerAbilityComponent::ApplyEffectToTargetDeferred);

	UGameplayStatics::FinishSpawningActor(Projectile, ProjectileTransform);
}

void AMagePlayerCharacter::AbilityMeteor()
{
	AbilityComps[MeteorIndex]->HideIndicator();
	AbilityComps[MeteorIndex]->ApplyEffectToSelf();

	AnimInstance->Montage_Play(MeteorMontage.LoadSynchronous());

	const FVector TargetLocation = AbilityComps[MeteorIndex]->GetTargetLocation() + FVector(0.0, 0.0, MeteorSpawnHeight);

	// Spawn the meteor
	MeteorParticleSystem = 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorVFX.LoadSynchronous(), TargetLocation);

	FTimerHandle DurationTimer;

	GetWorldTimerManager().SetTimer(DurationTimer, this, &AMagePlayerCharacter::OnMeteorImpact, 0.5, false);
}

void AMagePlayerCharacter::OnMeteorImpact()
{
	MeteorParticleSystem->DeactivateSystem();

	AbilityComps[MeteorIndex]->ApplyEffectDeferred();

	AbilityComps[MeteorIndex]->EndAbility();
}

void AMagePlayerCharacter::AbilityBlackhole()
{
	AnimInstance->Montage_Play(BlackholeMontage.LoadSynchronous());
}

void AMagePlayerCharacter::SpawnBlackhole()
{
	AbilityComps[BlackholeIndex]->HideIndicator();

	const FVector TargetLocation = AbilityComps[BlackholeIndex]->GetTargetLocation();

	// Spawn the blackhole
	UParticleSystemComponent* ParticleSystem =
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlackholeVFX.LoadSynchronous(), TargetLocation);

	ParticleSystem->OnSystemFinished.AddDynamic(this, &AMagePlayerCharacter::OnBlackholeComplete);

	AbilityComps[BlackholeIndex]->ApplyEffectToSelf();

	FTimerDelegate ApplyEffectDelegate
		= FTimerDelegate::CreateUObject(AbilityComps[BlackholeIndex], &UPlayerAbilityComponent::ApplyEffectDeferred);

	GetWorldTimerManager().SetTimer(BlackholeApplyEffectTimer, ApplyEffectDelegate, BlackholeAbilityInfo.GetHitRate(), true);
}

void AMagePlayerCharacter::OnBlackholeComplete(UParticleSystemComponent* PSystem)
{
	GetWorldTimerManager().ClearTimer(BlackholeApplyEffectTimer);

	AbilityComps[BlackholeIndex]->EndAbility();
}

void AMagePlayerCharacter::AbilityBurden()
{
	AbilityComps[BurdenIndex]->ApplyEffectToSelf();

	AnimInstance->Montage_Play(BurdenMontage.LoadSynchronous());
}

void AMagePlayerCharacter::SpawnBurdenProjectile()
{
	const FVector ProjectilePos = GetMesh()->GetSocketLocation(BasicAttackSpawnSocket);
	const FRotator ProjectileRot = GetActorRotation();
	
	// Starting projectile transform
	const FTransform ProjectileTransform = FTransform(ProjectileRot, ProjectilePos);

	AProjectile* Projectile =
		GetWorld()->SpawnActorDeferred<AProjectile>(BurdenProjectileClass.LoadSynchronous(), ProjectileTransform);

	Projectile->Init(ProjectileTransform.GetRotation().Vector(), AIdkEnemyCharacter::StaticClass(), this);
	Projectile->OnCollisionDelegate.BindUObject(
		AbilityComps[BurdenIndex], &UPlayerAbilityComponent::ApplyEffectToTargetDeferred);

	UGameplayStatics::FinishSpawningActor(Projectile, ProjectileTransform);
}

void AMagePlayerCharacter::OnBurdenComplete()
{
	AbilityComps[BurdenIndex]->EndAbility();
}
