// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkCharacter.h"
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "IdkEnemyCharacter.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FObjectInitializer;
class AActor;
class AIdkPlayerCharacter;
class UAbilityComponent;
class UAIAbilityComponent;
class UAnimMontage;
class UBlackboardComponent;
class UCurveTable;
class UHealthAndStatusBarWidget;
class UOrientToPlayerComponent;
class UOverheadWidgetComponent;
class UPhysicalAnimationComponent;
struct FAbilityInfo;

DECLARE_DELEGATE_TwoParams(FOnEnemyDeathDelegate, int32, int32);

/** Character subclass used as a base for all enemies. */
UCLASS(Blueprintable)
class AIdkEnemyCharacter : public AIdkCharacter
{
	GENERATED_BODY()

public:
	AIdkEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	//~ End AActor Interface

	//~ Begin AIdkCharacter Interface.
	virtual void SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget) override;
	//~ End AIdkCharacter Interface

	/**
	 * Initialize the enemy.
	 * 
	 * @param InLevel The level of the enemy
	 */
	void Init(const uint8 InLevel);

	/** Sets whether the enemy is stunned or not. */
	void SetStunned(bool bStunned) const;

	/**
	 * Activate one of the enemy's abilities.
	 * 
	 * @param Target The target of the ability.
	 * @param Index The index of the ability to activate.
	 */
	void ActivateAbility(const AActor* Target, const int32 Index);

	/** 
	 * Event called whenever an ability the enemy used completes. 
	 * 
	 * @param Index The index of the ability that is ending.
	 */
	void OnAbilityEnd(const int32 Index);

	/**
	 * Sets the blackboard key that represents if an ability is on cooldown or not.
	 * 
	 * @param bOnCooldown Whether the ability should be on cooldown or not.
	 * @param Index The index of the ability to set the blackboard key for.
	 */
	void SetAbilityCooldownBBKey(const bool bOnCooldown, const int32 Index);

	/**
	 * Get a location that is in range of the target for the specified ability.
	 * 
	 * @param Target The target of the ability.
	 * @param Index The index of the ability.
	 */
	UE_NODISCARD FVector GetPosInAbilityRange(const AActor* Target, const int32 Index) const;

	/**
	 * Check whether the enemy is currently in range of the target for the specified ability.
	 * 
	 * @param Target The target of the ability.
	 * @param Index The index of the ability.
	 * @return True if the enemy is in range, false otherwise.
	 */
	UE_NODISCARD bool IsInAbilityRange(const AActor* Target, const int32 Index) const;

	/**
	 * Checks if any additional conditions are fulfilled for the specified ability.
	 * Calls AreAbilityConditionsFufilledImpl, which can be overridden by subclasses to add custom ability activation conditions.
	 * 
	 * @param Index The index of the ability.
	 * @return True if the conditions are fulfilled, false otherwise.
	 */
	UE_NODISCARD bool AreAbilityBonusConditionsFulfilled(const int32 Index) const;
	
	/**
	 * Delegate called when the enemy dies.
	 * 
	 * @param Experience awarded to the player on death.
	 * @param Gold awarded to the player on death.
	 */
	FOnEnemyDeathDelegate OnDeathDelegate;

protected:
	//~ Begin AIdkCharacter Interface.
	UE_NODISCARD virtual uint8 GetLevel() const override final;
	virtual void OnDeath() override;
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted) override;
	//~ End AIdkCharacter Interface

	/** Function that gives subclasses an opportunity to bind ability delegates before abilities are assigned to ability components. */
	virtual void BindAbilities();

	/** Function called after ability components are created. */
	virtual void PostAbilityComponentsCreated();

	/** Implementation of the enemy's basic attack. Must be overriden by subclasses. */
	UFUNCTION()
	virtual void BasicAttack() PURE_VIRTUAL(&AIdkEnemyCharacter::BasicAttack, ;);

	/**
	 * Checks if any additional conditions are fulfilled for the specified ability.
	 * Can be overridden by subclasses to add custom ability activation conditions.
	 *
	 * @param Index The index of the ability.
	 * @return True if the conditions are fulfilled, false otherwise.
	 */
	UE_NODISCARD virtual bool AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const;

	/** Level of the enemy. Affects health, as well as xp and gold on death. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 Level = 1;

	/** Ability components for each ability in the enemy's ability pool. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<UAIAbilityComponent*> AbilityComps;

	/** Component that handles blending ragdoll physics with death animations. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPhysicalAnimationComponent> PhysicalAnimationComp;

	/** Whether to ragdoll on death or not. Disable if mesh doesn't have a proper physics asset. */
	UPROPERTY(EditDefaultsOnly)
	bool bRagdollOnDeath = true;

	/** The player character. */
	TObjectPtr<AIdkPlayerCharacter> Player;

	/** The enemy's blackboard component */
	TObjectPtr<UBlackboardComponent> Blackboard;

	/** Index of the basic attack ability component within AbilityComps */
	static const int32 BasicAttackAbilityIndex = 0;

private:
	/** Enable ragdoll physics for this enemy. */
	void EnableRagdoll();

	/** Disable ragdoll physics for the enemy. */
	void DisableRagdoll();

	/** Update the blend amount for the death animation and ragdoll physics. */
	void UpdateDeathBlend();

	/** Component that displays the enemy's health and active status effects over their heads. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOverheadWidgetComponent> HealthAndStatusBarWidgetComp;

	/** Component that always faces the player. @see UOrientToPlayerComponent */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOrientToPlayerComponent> OrientToPlayerComp;

	/** Curve table containing curves for xp and gold on death, based on level. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UCurveTable> CurveTable;

	/** How often to update the blend amount when transitioning from death animation to ragdoll. Set to a smaller number for shorter death animations. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bRagdollOnDeath", EditConditionHides))
	float DeathBlendInterval = 0.1f;

	/** How long to blend the death animation with ragdoll physics. Set automatically based on the blend out duration of the death animation. */
	float DeathBlendDuration = 0.f;

	/** Remaining duration for blending death animation with ragdoll physics. */
	float DeathBlendDurationRemaining = 0.f;

	/** How long to ragdoll before freezing pose. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bRagdollOnDeath", EditConditionHides))
	float RagdollDuration = 1.f;

	/** Timer that handles updating the blend amount when transitioning from death animation to ragdoll. */
	FTimerHandle DeathBlendTimer;

	/** Names of the blackboard keys that represent whether an ability is on cooldown. */
	static const TArray<FName> AbilityCooldownBBKeys;

	/** Names of the blackboard keys that represent whether a specific ability is being used. */
	static const TArray<FName> AbilityBBKeys;

	/** Name of the blackboard key that represents whether the enemy is stunned or not. */
	static const FName StunnedKey;

	/** Name of the blackboard key that represents whether the enemy is using an ability or not. */
	static const FName IsUsingAnyAbilityKey;

	/** Name of the collision profile to use for ragdolling. */
	static const FName RagdollCollisionProfile;

	/** Name of the curve within CurveTable that represents the amount of experience awarded to the player when this enemy dies. */
	static const FName ExperienceCurve;

	/** Name of the curve within CurveTable that represents the amount of gold awarded to the player when this enemy dies. */
	static const FName GoldCurve;

};
