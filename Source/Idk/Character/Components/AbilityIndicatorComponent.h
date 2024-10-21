// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/DecalComponent.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilityIndicatorComponent.generated.h"

class AIdkPlayerCharacter;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UObject;
struct FAbilityTargetingInfo;

DECLARE_DELEGATE_RetVal(FVector, FGetAimLocationDelegate);

/**
 * 
 */
UCLASS(NotBlueprintable)
class UAbilityIndicatorComponent final : public UDecalComponent
{
	GENERATED_BODY()

public:
	UAbilityIndicatorComponent();

	/** Update the position of the indicator to match where the owner is aiming. */
	void UpdateIndicator();

	/**
	 * Set up the indicator based on the specified targeting info.
	 * 
	 * @param AbilityTargetingInfo Targeting info for the ability.
	 */
	void SetupIndicator(const FAbilityTargetingInfo& AbilityTargetingInfo);

	/** Set whether the indicator is greyed out. */
	void SetGreyedOut(const bool bGreyedOut);

	/** Applies an offset to this component relative to its parent depending on the ability's area type. */
	void ApplyOffset();

	/**
	 * Gets the location that the owner is aiming at.
	 * 
	 * @return AimLocation Location where the owner is aiming.
	 */
	FGetAimLocationDelegate GetAimLocationDelegate;

private:
	/**
	 * Set up the indicator for a circle.
	 * 
	 * @param Radius Radius of the circle.
	 */
	void SetupCircleIndicator(const double Radius);

	/**
	 * Set up the indicator for a cone.
	 * 
	 * @param Radius	Radius of the cone.
	 * @param HalfAngle Half angle of the cone in degrees.
	 */
	void SetupConeIndicator(const double Radius, const double HalfAngle);

	/**
	 * Set up the indicator for a rectangle.
	 *
	 * @param Width		Width of the rectangle.
	 * @param Length	Length of the rectangle.
	 */
	void SetupRectangleIndicator(const double Width, const double Length);

	/** Get the material for the indicator. */
	UE_NODISCARD static UMaterialInterface& GetIndicatorMaterial(const UObject* WorldContextObject);

	/** Material instance used by the indicator. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMaterialInstanceDynamic> DynamicIndicatorMaterial;

	/** Targeting info of the ability. */
	const FAbilityTargetingInfo* TargetingInfo = nullptr;

	/** Name of the material parameter that controls how grey the material is. */
	static const FName IndicatorGreyPercent;

	/** Name of the material parameter that controls whether the indicator is a circle. */
	static const FName IndicatorIsCircle;

	/** Name of the material parameter that controls whether the indicator is a cone. */
	static const FName IndicatorIsCone;

	/** Name of the material parameter that controls the half angle for cones. */
	static const FName IndicatorHalfAngle;

};
