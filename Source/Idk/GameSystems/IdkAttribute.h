// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "IdkAttribute.generated.h"

class FName;

/** Represents the type of an attribute or modifer for a character. */
UENUM()
enum class EAttributeType : uint8
{
	None UMETA(Hidden),
	Health,
	MovementSpeed,
	Stun,

	/** Affects damage done. */
	Damage,

	/** Affects healing done. */
	Healing,

	/** Negative values act as damage resistance. */
	DamageTaken,
};

/** Represents an attribute or modifer for a character. */
USTRUCT()
struct FAttribute final
{
	GENERATED_BODY()

public:
	FAttribute() = default;

	/**
	 * @param bTreatAsModifier	Whether to treat the attribute as a modifier (modifiers don't have a base value). 
	 */
	FAttribute(const bool bTreatAsModifier);

	/** Copy constructor. */
	FAttribute(const FAttribute& Other);

	FAttribute& operator+=(const FAttribute& Other);
	UE_NODISCARD FAttribute operator*(const double Multiplier) const;

	/**
	 * Initialize the attribute.
	 * 
	 * @param InAttribute		Type of the attribute. 
	 * @param InBaseValue		Base value of the attribute.
	 * @param InBonus			Bonus to the attribute's value.
	 * @param InMultiplierBonus	Bonus to the attribute's multiplier. 
	 */
	void Init(const EAttributeType InAttribute, const double InBaseValue = 0.0, const double InBonus = 0.0, const double InMultiplierBonus = 0.0);

	/** Update the base value of the attribute. */
	void Update(const double InBaseValue);

	/** Add value to attribute's current bonus. */
	void AddBonus(const double InBonus);

	/** Add value to attribute's current multiplier bonus. */
	void AddMultiplierBonus(const double InMultiplierBonus);

	/** Get the calculated final value of the attribute. */
	UE_NODISCARD double GetFinalValue() const;

	/**
	 * Apply the modifier to the specified value.
	 * 
	 * @param Value	Value to apply modifiers to. 
	 * @return		Value after modifiers have been applied.
	 */
	UE_NODISCARD double ApplyModifier(const double Value) const;

	/** Get the type of the attribute. */
	UE_NODISCARD EAttributeType GetAttributeType() const;

	/** Get the base value of the attribute. */
	UE_NODISCARD double GetBaseValue() const;

	/** Get the current bonus of the attribute. */
	UE_NODISCARD double GetBonus() const;

	/** Get the current multiplier bonus of the attribute. */
	UE_NODISCARD double GetMultiplierBonus() const;

	/** Check if the attribute can have a base value. */
	UE_NODISCARD bool CanHaveBaseValue() const;

	/** Check if the attribute can have a bonus. */
	UE_NODISCARD bool CanHaveBonus() const;

	/** Check if the attribute can have a multiplier bonus. */
	UE_NODISCARD bool CanHaveMultiplierBonus() const;

#if WITH_EDITOR
	/**
	 * Can be called from a UObject's PostEditChangeProperty() to update the attribute.
	 * 
	 * @param PropertyName			Name of the property that was changed.
	 * @param bInTreatAsModifier	Whether to treat the attribute as a modifier (modifiers don't have a base value). 
	 */
	void PostEditChangeProperty(const FName& PropertyName, const bool bInTreatAsModifier);
#endif

	/** Check if the attribute type is considered a modifier. */
	UE_NODISCARD static constexpr bool IsAModifier(const EAttributeType AttributeType)
	{
		return !CanAttributeHaveBaseValue(AttributeType);
	}

	/** Check if the attribute type can have a base value. */
	UE_NODISCARD static constexpr bool CanAttributeHaveBaseValue(const EAttributeType AttributeType)
	{
		switch (AttributeType)
		{
			case EAttributeType::Damage:
			case EAttributeType::Healing:
			case EAttributeType::DamageTaken:
				return false;
			default:
				break;
		}

		return true;
	}

	/** Check if the attribute type can have a bonus. */
	UE_NODISCARD static constexpr bool CanAttributeHaveBonus(const EAttributeType AttributeType)
	{
		switch (AttributeType)
		{
			case EAttributeType::MovementSpeed:
				return false;
			default:
				break;
		}

		return true;
	}

	/** Check if the attribute type can have a multiplier bonus. */
	UE_NODISCARD static constexpr bool CanAttributeHaveMultiplierBonus(const EAttributeType AttributeType)
	{
		switch (AttributeType)
		{
			case EAttributeType::Stun:
				return false;
			default:
				break;
		}

		return true;
	}

private:
	/**
	 * @param AttributeType		Type of the attribute.			 
	 * @param BaseValue			Base value of the attribute.
	 * @param Bonus				Bonus to the attribute's value.
	 * @param MultiplierBonus	Bonus to the attribute's multiplier.
	 * @param bTreatAsModifier	Whether to treat the attribute as a modifier.
	 */
	FAttribute(const EAttributeType AttributeType, const double BaseValue, const double Bonus, const double MultiplierBonus, const bool bTreatAsModifier);

	/** Set values to defaults if they can't be used by the attribute type. */
	void ResetUnusedValues();

	/** Type of the attribute. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bTreatAsModifier", EditCondtionHides, HideEditConditionToggle, InvalidEnumValues = "None"))
	EAttributeType AttributeType = EAttributeType::None;

	/** Whether the attribute can have a base value. */
	UPROPERTY()
	bool bCanHaveBaseValue = true;

	/** Whether the attribute can have a bonus. */
	UPROPERTY()
	bool bCanHaveBonus = false;

	/** Whether the attribute can have a multiplier bonus. */
	UPROPERTY()
	bool bCanHaveMultiplier = false;

	/** Base value of the attribute. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", EditCondition = "bCanHaveBaseValue", EditConditionHides, HideEditConditionToggle))
	double BaseValue = 0.0;

	/** Bonus to the attribute's value. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanHaveBonus", EditConditionHides, HideEditConditionToggle))
	double Bonus = 0.0;

	/** Bonus to the attribute's multiplier. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0", EditCondition = "bCanHaveMultiplier", EditConditionHides, HideEditConditionToggle))
	double MultiplierBonus = 0.0;

	/** Whether to treat the attribute as a modifer. */
	UPROPERTY()
	bool bTreatAsModifier = false;

	/** Minimum value for the sum of BaseValue and Bonus before the multiplier is applied. */
	static constexpr double MinPreMultValue = 1.0;

	/** Minimum value for the result of (BaseValue + Bonus) * (1.0 + MultiplierBonus). */
	static constexpr double MinPostMultValue = 1.0;

	/** Minimum value for the sum of 1.0 and MultiplierBonus. */
	static constexpr double MinMultiplier = 0.1;
};
