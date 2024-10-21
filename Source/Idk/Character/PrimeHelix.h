// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/EliteEnemyCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "PrimeHelix.generated.h"

enum class EDataValidationResult : uint8;
class AProjectile;
class FDataValidationContext;
class FObjectInitializer;
class UAbilityComponent;
class UAnimMontage;
class UParticleSystem;
class UParticleSystemComponent;
struct FBranchingPointNotifyPayload;

/** Prime Helix elite enemy. */
UCLASS(Abstract, Blueprintable)
class APrimeHelix final : public AEliteEnemyCharacter
{
	GENERATED_BODY()
	
public:
	APrimeHelix(const FObjectInitializer& ObjectInitializer);
	
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

private:
	//~ Begin AIdkCharacter Interface.
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override final;
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	//~ End AIdkCharacterInterface

	//~ Begin AIdkEnemyCharacter Interface.
	virtual void BindAbilities() override;
	virtual void PostAbilityComponentsCreated() override;
	virtual void BasicAttack() override;
	UE_NODISCARD virtual bool AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const override;
	//~ End AIdkEnemyCharacter Interface

	/** Event called when the basic attack has finished charging. */
	void OnBasicAttackChargeEnd();

	/** Event called when the basic attack has finished firing. */
	UFUNCTION()
	void OnBasicAttackFireEnd(UParticleSystemComponent* PSystem);

	/** Implementation for the barrage ability. */
	void AbilityBarrage();

	/** Event called when barrage has finished charging. */
	void OnBarrageChargeEnd();

	/** Implementation for the shockwave ability. */
	void AbilityShockwave();

	/** Event called when shockwave has finished charging. */
	void OnShockwaveChargeEnd();

	/** Implementation for the slam ability. */
	void AbilitySlam();

	/** Event called when slam has finished charging. */
	void OnSlamChargeEnd();

	/** VFX used by the basic attack. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack")
	TSoftObjectPtr<UParticleSystem> BasicAttackVFX;

	/** Name of the montage notify that causes the basic attack to fire. */
	const FName BasicAttackChargeEndNotify = TEXT("BasicAttackChargeEnd");

	/** Name of the socket representing the origin of the basic attack. */
	const FName BasicAttackOriginSocket = TEXT("Muzzle_Front");

	/** Ability info for the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Info"))
	FAbilityInfo BarrageAbilityInfo;

	/** Animation used for the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> BarrageMontage;

	/** Blueprint class of the projectile used by the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Projectile Class", BlueprintBaseOnly))
	TSoftClassPtr<AProjectile> BarrageProjectileClass;

	/** Number of projectiles to spawn for the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Number of Projectiles", ClampMin = "1"))
	uint8 BarrageNumProjectiles = 5;

	/** Angle representing the maximum spread of projectiles spawned by the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Spread HalfAngle", ClampMin = "1.0", ClampMax = "90.0", Units = "Degrees"))
	double BarrageSpreadHalfAngle = 30.0;

	/** Starting vertical angle of projectiles spawned by the barrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Launch Angle", ClampMin = "0.0", ClampMax = "90.0", Units = "Degrees"))
	double BarrageLaunchAngle = 0.0;

	/** When health percentage is below the threshold, barrage can be activated. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Barrage", meta = (DisplayName = "Activation Health Threshold", ClampMin = "0.0", ClampMax = "1.0"))
	double BarrageHealthThreshold = 0.5;

	/** Name of the montage notify that causes barrage to spawn its projectiles. */
	const FName BarrageFireNotify = TEXT("Barrage");

	/** Name of the socket to spawn barrage's projectiles at. */
	const FName BarrageOriginSocket = TEXT("Muzzle_02");

	/** Index of the ability component used by the barrage ability. */
	int32 BarrageIndex = -1;

	/** Ability info for the shockwave ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Shockwave", meta = (DisplayName = "Info"))
	FAbilityInfo ShockwaveAbilityInfo;

	/** Animation used for the shockwave ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Shockwave", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> ShockwaveMontage;

	/** Name of the montage notify that causes shockwave to activate. */
	const FName ShockwaveChargeEndNotify = TEXT("Shockwave");

	/** Name of the socket representing the origin of the shockwave ability. */
	const FName ShockwaveCollisionSocket = TEXT("FX_CenterMass");

	/** Index of the ability component used by the shockwave ability. */
	int32 ShockwaveIndex = -1;

	/** Ability info for the slam ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Slam", meta = (DisplayName = "Info"))
	FAbilityInfo SlamAbilityInfo;

	/** Animation used for the slam ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Slam", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> SlamMontage;

	/** VFX for the slam ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Slam", meta = (DisplayName = "Ground VFX"))
	TSoftObjectPtr<UParticleSystem> SlamGroundVFX;

	/** Name of the montage notify that fires when the Prime Helix's hands hit the ground. */
	const FName SlamImpactNotify = TEXT("Slam");

	/** Name of the socket representing the origin of the slam ability. */
	const FName SlamCollisionSocket = TEXT("FX_MeleeImpact");

	/** Index of the ability component used by the slam ability. */
	int32 SlamIndex = -1;

};
