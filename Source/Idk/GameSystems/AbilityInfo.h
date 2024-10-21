// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/UI/GenericDisplayInfo.h"
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilityInfo.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FString;
class UAbilityComponent;
class UAttributeSystemComponent;
class UMultiStageEffect;

/** Represents the type of an ability's area of effect. */
UENUM()
enum class EAbilityAreaType : uint8
{
	None UMETA(Hidden),
	Circle,
	Cone,
	Rectangle,

	/** Ability will handle its own collisions. */
	Custom,
};

/** Information used by ability collisions and indicators. */
USTRUCT()
struct FAbilityTargetingInfo final
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	/** @see UObject::IsDataValid */
	UE_NODISCARD EDataValidationResult IsDataValid(FDataValidationContext& Context) const;
#endif

	/** Get the area type of the ability. */
	UE_NODISCARD EAbilityAreaType GetAreaType() const;

	/** Check whether the ability uses ranged targeting. */
	UE_NODISCARD bool UsesRangedTargeting() const;

	/** Check whether the ability uses ranged activation. */
	UE_NODISCARD bool UsesRangedActivation() const;

	/** Get the range of the ability. */
	UE_NODISCARD double GetRange() const;

	/** Get the radius of the area of effect. For cones and circles. */
	UE_NODISCARD double GetRadius() const;

	/** Get the half-angle of area of effect. For cones. */
	UE_NODISCARD double GetHalfAngle() const;

	/** Get the width of the area of effect. For rectangles. */
	UE_NODISCARD double GetWidth() const;

	/** Get the length of the area of effect. For rectangles. */
	UE_NODISCARD double GetLength() const;

	/** Check whether the ability handles its own collisions. */
	UE_NODISCARD bool HandlesCollision() const;

	/** Check whether the ability has infinite range. */
	UE_NODISCARD bool HasInfiniteRange() const;

	/**
	 * Create targeting info for a circular area of effect.
	 * 
	 * @param Radius			Radius of the circle.
	 * @param Range				Range of the ability.
	 * @param bRangedTargeting	Whether the ability uses ranged targeting.
	 * @param bRangedActivation	Whether the ability uses ranged activation.
	 * @return					Created ability targeting info.
	 */
	UE_NODISCARD static FAbilityTargetingInfo InitCircle(const double Radius, const double Range, const bool bRangedTargeting, const bool bRangedActivation);

	/**
	 * Create targeting info for a conical area of effect
	 * 
	 * @param Radius			Radius of the cone.
	 * @param HalfAngle			Half-angle of the cone
	 * @param Range				Range of the ability.
	 * @param bRangedTargeting	Whether the ability uses ranged targeting.
	 * @param bRangedActivation	Whether the ability uses ranged activation.
	 * @return					Created ability targeting info.
	 */
	UE_NODISCARD static FAbilityTargetingInfo InitCone(const double Radius, const double HalfAngle, const double Range, const bool bRangedTargeting, const bool bRangedActivation);

	/**
	 * Create targeting info for a rectangular area of effect.
	 * 
	 * @param Width				Width of the rectangle.
	 * @param Length			Length of the rectangle.
	 * @param Range				Range of the ability.
	 * @param bRangedTargeting	Whether the ability uses ranged targeting.
	 * @param bRangedActivation	Whether the ability uses ranged activation.
	 * @return					Created ability targeting info.
	 */
	UE_NODISCARD static FAbilityTargetingInfo InitRectangle(const double Width, const double Length, const double Range, const bool bRangedTargeting, const bool bRangedActivation);

	/**
	 * Create targeting info for a custom area of effect.
	 * 
	 * @param Range				Range of the ability.
	 * @param bRangedTargeting	Whether the ability uses ranged targeting.
	 * @param bRangedActivation	Whether the ability uses ranged activation.
	 * @return					Created ability targeting info.
	 */
	UE_NODISCARD static FAbilityTargetingInfo InitCustom(const double Range = 0.0, const bool bRangedTargeting = false, const bool bRangedActivation = false);

private:
	/** Type of area of effect used by the ability. */
	UPROPERTY(EditDefaultsOnly)
	EAbilityAreaType AreaType = EAbilityAreaType::None;

	/** Whether the ability uses ranged targeting. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType != EAbilityAreaType::None"))
	bool bRangedTargeting = false;

	/** Whether the ability uses ranged activation. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bRangedTargeting"))
	bool bRangedActivation = false;

	/** Range of the ability. When negative, range is infinite. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType != EAbilityAreaType::None", ClampMin = "-1.0"))
	double Range = 0.0;

	/** Radius of the area of effect. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType == EAbilityAreaType::Circle || AreaType == EAbilityAreaType::Cone", ClampMin = "0.0"))
	double Radius = 0.0;

	/** Half-angle of the cone. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType == EAbilityAreaType::Cone", ClampMin = "0.0", ClampMax = "90.0"))
	double HalfAngle = 0.0;

	/** Width of the area of effect. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType == EAbilityAreaType::Rectangle", ClampMin = "0.0"))
	double Width = 0.0;

	/** Length of the area of effect. */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AreaType == EAbilityAreaType::Rectangle", ClampMin = "0.0"))
	double Length = 0.0;
};

/** Information representing an ability. */
USTRUCT()
struct FAbilityInfo final
{
	GENERATED_BODY()

	friend class FAbilityInfoBuilder;
	
public:
#if WITH_EDITOR
	/** @see UObject::IsDataValid */
	UE_NODISCARD EDataValidationResult IsDataValid(FDataValidationContext& Context) const;
#endif

	/** Initialize the ability's description based on modifiers from the specified attribute system. */
	void InitDescription(UAttributeSystemComponent& AttributeSystem);

	/** Get the ability's name. */
	UE_NODISCARD const FName& GetName() const;

	/** Get the information used to display the ability to the player. */
	UE_NODISCARD const FGenericDisplayInfo& GetDisplayInfo() const;

	/** Check if the ability has a cooldown. */
	UE_NODISCARD bool HasCooldown() const;

	/** Check if the ability's cooldown is infinite. */
	UE_NODISCARD bool IsCooldownInfinite() const;

	/** Get the ability's cooldown. */
	UE_NODISCARD double GetCooldown() const;

	/** Get the ability's targeting info. */
	UE_NODISCARD const FAbilityTargetingInfo& GetTargetingInfo() const;

	/** Get the ability's effect. */
	UE_NODISCARD const UMultiStageEffect& GetEffect() const;

	/** Get the ability's effect. */
	UE_NODISCARD UMultiStageEffect& GetEffect();

	/** For abilities that hit multiple times, get the number of hits per second. */
	UE_NODISCARD double GetHitRate() const;

	/** Get the duration of the ability. */
	UE_NODISCARD double GetDuration() const;

	/** HitRate / Duration */
	UE_NODISCARD double GetPerHitMultiplier() const;

	/** Delegate that activates the ability. Should be bound to the ability's implementation. */
	FSimpleDelegate ActivateAbilityDelegate;

private:
	/** Name of the ability. */
	UPROPERTY(EditAnywhere)
	FName Name;

	/** Information used to display the ability to the player. */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FGenericDisplayInfo DisplayInfo;

	/** Cooldown of the ability. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0"))
	double Cooldown = 0.0;

	/** Targeting info for the ability. */
	UPROPERTY(EditAnywhere)
	FAbilityTargetingInfo TargetingInfo;

	/** Effect of the ability. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TObjectPtr<UMultiStageEffect> Effect;

	/** Duration of the ability. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	double Duration = 0.0;

	/** For abilities that hit multiple times, the number of hits per second. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	double HitRate = 0.0;

#if WITH_EDITOR
	/** Prefix for error text during data validation. */
	static const FText DataValidationPrefix;
#endif
};
