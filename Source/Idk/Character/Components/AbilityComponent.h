// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/SceneComponent.h>
#include <Engine/EngineBaseTypes.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilityComponent.generated.h"

class AIdkCharacter;
class UAbilityCollisionComponent;
class UAbilityIndicatorComponent;
class UAttributeSystemComponent;
class USkeletalMeshComponent;
struct FAbilityInfo;

/**
 * Component that manages an ability usable by the equipped character. 
 * Handles collisions, showing and hiding indicators, and cooldowns.
 */
UCLASS(Abstract, NotBlueprintable)
class UAbilityComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UAbilityComponent();

	//~ Begin UActorComponent Interface.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	/** Set the ability for this component. */
	virtual void SetAbility(const FAbilityInfo& Ability);

	/** Whether or not the ability indicator can be shown. */
	UE_NODISCARD virtual bool CanShowIndicator() const;

	/** Show the ability indicator */
	virtual void ShowIndicator();

	/** Hide the ability indicator */
	virtual void HideIndicator();

	/** Marks the beginning of ability execution. */
	virtual void StartAbility();

	/** Marks the end of ability execution. */
	virtual void EndAbility();

	/** Set the attribute system component of the owning character. */
	void SetAttributeSystem(UAttributeSystemComponent* InOwnersAttributeSystem);

	/** Check if an ability has been set for this component. */
	UE_NODISCARD bool IsAbilitySet() const;

	/** Check if the ability is on cooldown. */
	UE_NODISCARD bool IsOnCooldown() const;

	/** Check if the ability indicator is currently being shown. */
	UE_NODISCARD bool IsIndicatorVisible() const;

	/**
	 * Applies any self effects to the owner's attribute system.
	 * If the ability component handles collisions, then it will also apply any 
	 * target effects to any targets in the ability's area of effect.
	 */
	void ApplyEffect();

	/** Applies any self effects to the owner's attribute system. */
	void ApplyEffectToSelf();

	/** Applies target effects to any targets in the ability's area of effect. */
	void ApplyEffectDeferred();

	/** Applies target effects to the specified target. */
	UFUNCTION()
	void ApplyEffectToTargetDeferred(UAttributeSystemComponent* TargetAttributeSystem);

	/**
	 * Get the location of the ability's target.
	 * For melee abilities, returns the ability of this component.
	 * For ranged abilities, calls GetRangedTargetLocation(), which must be overridden by subclasses.
	 */
	UE_NODISCARD FVector GetTargetLocation() const;

	/** Get the location of the ability's target from when the ability was first activated. */
	UE_NODISCARD FVector GetInitialTargetLocation() const;

	/** Returns the ability info of the equipped ability. */
	UE_NODISCARD const FAbilityInfo* GetAbilityInfo() const;

	/** Disables collisions for this ability. */
	void DisableCollisions();

protected:
	/** Get the location of the target for ranged abilities. Must be overridden by subclasses. */
	UE_NODISCARD virtual FVector GetRangedTargetLocation() const PURE_VIRTUAL(&UAbilityComponent::GetRangedTargetLocation, return FVector(););

	/** Get the character class of targets we wan't to be able to generate overlap events for. Must be overridden by subclasses. */
	UE_NODISCARD virtual TSubclassOf<AIdkCharacter> GetCollisionClassFilter() const PURE_VIRTUAL(&UAbilityComponent::GetCollisionClassFilter, return nullptr;);

	/** Get the location that the owning character is aiming at. Must be overridden by subclasses. */
	UE_NODISCARD virtual FVector GetAimLocation() const PURE_VIRTUAL(&UAbilityComponent::GetAimLocation, return FVector(););

	/** Component that handles collisions for the equipped ability. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilityCollisionComponent> CollisionComp;

	/** Component that displays an visual indicator of the ability's area of effect. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilityIndicatorComponent> IndicatorComp;

	/** Ability info of the equipped ability. */
	const FAbilityInfo* AbilityInfo = nullptr;

	/** Whether the ability is on cooldown. */
	UPROPERTY(VisibleInstanceOnly)
	bool bOnCooldown = false;

	/** Location of the ability's target from when the ability first activated. */
	FVector InitialTargetLocation;

	/** Location of the ability's target from when self effects were applied. */
	FVector ActivationLocation;

	/** Attribute system of the owning character. */
	TObjectPtr<UAttributeSystemComponent> OwnersAttributeSystem;

	/** Whether to update the indicator each tick. */
	bool bTickIndicator = false;

};
