// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/BoxComponent.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "AbilityCollisionComponent.generated.h"

class AIdkCharacter;
class UAttributeSystemComponent;
struct FAbilityTargetingInfo;

/** Component that handles collisions for ability components. */
UCLASS(NotBlueprintable)
class UAbilityCollisionComponent final : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UAbilityCollisionComponent();

	/**
	 * Set up collision based on the specified targeting info.
	 * 
	 * @param AbilityTargetingInfo Targeting info for the ability.
	 */
	void SetupCollision(const FAbilityTargetingInfo& AbilityTargetingInfo);

	/**
	 * Get the attribute systems of all overlapping characters matching the supplied filter.
	 * 
	 * @param OutOverlappingCharacters	Attribute systems of all overlapping characters matching the supplied filter.
	 * @param ClassFilter				Limit collisions to characters of the provided class.
	 */
	void GetOverlappingAttributeSystems(TArray<UAttributeSystemComponent*>& OutOverlappingCharacters, TSubclassOf<AIdkCharacter> ClassFilter);

	/** Applies an offset to this component relative to its parent depending on the ability's area type. */
	void ApplyOffset();

private:
	/**
	 * Gets the extents for a hitbox that encompasses a cone with the provided radius and half angle.
	 * 
	 * @param Radius	Radius of the cone.
	 * @param HalfAngle Half angle of the cone in degrees.
	 * @return 
	 */
	UE_NODISCARD static FVector GetExtentForCone(const double Radius, const double HalfAngle);

	/** Targeting info of the ability. */
	const FAbilityTargetingInfo* TargetingInfo = nullptr;

	/** Height of the hitbox. */
	static constexpr double CollisionHeight = 100.0;

	/** Name of the collision preset to use for the hitbox. */
	static const FName CollisionPreset;
};
