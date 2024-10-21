// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "MagePlayerCharacter.generated.h"

enum class EDataValidationResult : uint8;
class AProjectile;
class FDataValidationContext;
class FObjectInitializer;
class UAbilityComponent;
class UAnimMontage;
class UParticleSystem;
class UParticleSystemComponent;
class UPhysicalMaterial;
struct FBranchingPointNotifyPayload;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AMagePlayerCharacter : public AIdkPlayerCharacter
{
	GENERATED_BODY()
	
public:
	AMagePlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

private:
	//~ Begin AIdkCharacter Interface.
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override final;
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	//~ End AIdkCharacter Interface

	//~ Begin AIdkPlayerCharacter Interface.
	virtual void BasicAttackImpl() override final;
	//~ End AIdkPlayerCharacter Interface

	/** Spawns the projectile for the mage's basic attack. */
	void SpawnBasicAttackProjectile();

	/** Implementation for the meteor ability. */
	void AbilityMeteor();

	/** Event called when the meteor hits the ground. */
	void OnMeteorImpact();

	/** Implementation for the blackhole ability. */
	void AbilityBlackhole();

	/** Spawns the blackhole for the blackhole ability. */
	void SpawnBlackhole();

	/** Event called when the blackhole ability completes. */
	UFUNCTION()
	void OnBlackholeComplete(UParticleSystemComponent* PSystem);

	/** Implementatin for the burder ability. */
	void AbilityBurden();

	/** Spawns the projectile used for the burden ability. */
	void SpawnBurdenProjectile();

	/** Event called when the burden ability completes. */
	void OnBurdenComplete();

	/** Blueprint class for the projectile used for basic attacks. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (DisplayName = "Projectile Class", BlueprintBaseOnly))
	TSoftClassPtr<AProjectile> BasicAttackProjectileClass;

	/** Name of the mesh socket to spawn the basic attack projectile at. */
	static const FName BasicAttackSpawnSocket;

	/** Name of the montage notify that triggers when the basic attack projectile should be spawned. */
	static const FName BasicAttackSpawnNotify;

	/** Name of the montage notify that controls when the player can basic attack again. */
	static const FName CanAttackAgain;

	/** Ability info for the meteor ability. @see FAbilityInfo */
	UPROPERTY(EditAnywhere, Category = "Abilities|Meteor", meta = (DisplayName = "Info"))
	FAbilityInfo MeteorAbilityInfo;
	
	/** Animation used for the meteor ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Meteor", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> MeteorMontage;

	/** VFX for the meteor ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Meteor", meta = (DisplayName = "VFX"))
	TSoftObjectPtr<UParticleSystem> MeteorVFX;

	/** Height at which to spawn the meteor for the meteor ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Meteor", meta = (DisplayName = "Spawn Height"))
	double MeteorSpawnHeight = 700.0;

	/** Particle system component representing the meteor for the meteor ability. */
	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> MeteorParticleSystem;

	/** Index of the ability component used by the meteor ability. */
	int32 MeteorIndex = -1;

	/** Ability info for the blackhole ability. */
	UPROPERTY(EditAnywhere, Category = "Abilities|Blackhole", meta = (DisplayName = "Info"))
	FAbilityInfo BlackholeAbilityInfo;

	/** Animation used for the blackhole ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Blackhole", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> BlackholeMontage;

	/** VFX for the blackhole ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Blackhole", meta = (DisplayName = "VFX"))
	TSoftObjectPtr<UParticleSystem> BlackholeVFX;

	/** Timer that applies blackhole's effects to target at regular intervals for the duration. */
	FTimerHandle BlackholeApplyEffectTimer;

	/** Index of the ability component used by the blackhole ability. */
	int32 BlackholeIndex = -1;

	/** Name of the montage notify that triggers the spawning of the blackhole. */
	static const FName SpawnBlackholeNotify;

	/** Ability info for the burden ability. */
	UPROPERTY(EditAnywhere, Category = "Abilities|Burden", meta = (DisplayName = "Info"))
	FAbilityInfo BurdenAbilityInfo;

	/** Animation used for the burden ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Burden", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> BurdenMontage;

	/** Blueprint class for the projectile used by the burden ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Burden", meta = (DisplayName = "Projectile Class"))
	TSoftClassPtr<AProjectile> BurdenProjectileClass;

	/** Name of the montage notify that triggers the spawning of the projectile for the burden ability. */
	static const FName BurdenSpawnNotify;

	/** Index of the ability component used by the burden ability. */
	int32 BurdenIndex = -1;

};
