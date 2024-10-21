// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/SingleStageEffectGroup.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "ComplexEffect.generated.h"

enum class EConditionComparator : uint8;
enum class EDataValidationResult : uint8;
enum class EEffectType : uint8;
class FDataValidationContext;
class UAttributeSystemComponent;
class UObject;
class USimpleEffect;
struct FApplyEffectParams;

/** Represents the type of condition used by a complex effect. */
UENUM()
enum class EConditionType : uint8
{
	None,

	/** Condition checks the target's health percentage. */
	HealthPercentComparison,

	/** Condition checks if the target has a specific status effect. */
	StatusEffectExists,
};

/** Represents a condition for a complex effect. */
USTRUCT()
struct FEffectCondition final
{
	GENERATED_BODY()

	friend class UComplexEffect;
	friend class FComplexEffectBuilder;

public:
	/**
	 * Check whether the condition is fulfilled.
	 * 
	 * @param Target	Target to check the condition for. 
	 * @return			True if the condition is fulfilled, otherwise false.
	 */
	UE_NODISCARD bool IsActive(const UAttributeSystemComponent& Target) const;

	/**
	 * Check whether the condition is valid.
	 * 
	 * @param OutInvalidReason	Reason the condition is invalid. 
	 * @return					True if the condition is valid, otherwise false.
	 */
	UE_NODISCARD bool IsValid(FString& OutInvalidReason) const;

private:
	/** Type of condition to use. @see EConditionType */
	UPROPERTY(EditAnywhere)
	EConditionType Type = EConditionType::None;

	/** Value to compare against. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "Type == EConditionType::HealthPercentComparison", EditConditionHides))
	double Comparand = 0.0;

	/** Type of comparison. */
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EConditionComparator, EditCondition = "Type == EConditionType::HealthPercentComparison", EditConditionHides))
	uint8 Comparator = 0;

	/** Name of the status effect to check for. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EConditionType::StatusEffectExists", EditConditionHides))
	FName StatusEffectToCheckFor = FName();
};

/** Group of effects that is only applied if the condition is fulfilled. */
UCLASS(EditInlineNew)
class UComplexEffect final : public USingleStageEffectGroup
{
	GENERATED_BODY()

	friend class FComplexEffectBuilder;

public:
	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override final;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	//~ Begin USingleStageEffect Interface.
	virtual void ApplyEffect(FApplyEffectParams& Params) const override final;
	//~ End USingleStageEffect Interface

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/** Condition that must be fulfilled for subeffects to be applied. */
	UPROPERTY(EditAnywhere)
	FEffectCondition Condition;

};
