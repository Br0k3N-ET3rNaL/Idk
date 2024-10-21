// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/ComplexEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/EffectSystem/ConditionComparator.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

bool FEffectCondition::IsActive(const UAttributeSystemComponent& Target) const
{
	bool bConditionActive = false;

	switch (Type)
	{
		case EConditionType::HealthPercentComparison:
			bConditionActive = Target.DoesHealthPercentFulfillComparison(Comparand, (EConditionComparator)Comparator);
			break;
		case EConditionType::StatusEffectExists:
			bConditionActive = Target.HasStatusEffect(StatusEffectToCheckFor);
			break;
		default:
			break;
	}

	return bConditionActive;
}

bool FEffectCondition::IsValid(FString& OutInvalidReason) const
{
	switch (Type)
	{
		case EConditionType::HealthPercentComparison:
			if (Comparand < 0.0)
			{
				OutInvalidReason = TEXT("FEffectCondition: Comparand was less than 0.0, but Health % can never be less than 0%");

				return false;
			}
			else if (Comparand > 1.0)
			{
				OutInvalidReason = TEXT("FEffectCondition: Comparand was greater than 1.0, but Health % can never be greater than 100%");

				return false;
			}
			else if (Comparator == 0)
			{
				OutInvalidReason = TEXT("FEffectCondition: Condition uses comparison, but Comparator was unset");

				return false;
			}
			else if (((EConditionComparator)Comparator & EConditionComparator::LessThan) != EConditionComparator::None && Comparand == 0.0)
			{
				OutInvalidReason = TEXT("FEffectCondition: Comparator was < and Comparand was 0.0, but Health % can never be less than 0%");

				return false;
			}
			else if (((EConditionComparator)Comparator & EConditionComparator::GreaterThan) != EConditionComparator::None && Comparand == 1.0)
			{
				OutInvalidReason = TEXT("FEffectCondition: Comparator was > and Comparand was 1.0, but Health % can never be greater than 100%");

				return false;
			}
			break;
		case EConditionType::StatusEffectExists:
			if (StatusEffectToCheckFor.IsNone())
			{
				OutInvalidReason = TEXT("FEffectCondition: StatusEffectExists needs StatusEffectToCheckFor to be set");

				return false;
			}
			break;
		default:
			break;
	}

	return true;
}

const FString UComplexEffect::EffectClassName = TEXT("CmplxEffect");

#if WITH_EDITOR
EDataValidationResult UComplexEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	FString ConditionInvalidReason;

	if (!Condition.IsValid(ConditionInvalidReason))
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("UComplexEffect: Condition is invalid: %s"), *ConditionInvalidReason)));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

UEffect* UComplexEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UComplexEffect* NewEffect = DuplicateBaseEffect<UComplexEffect>(Outer, Name);

	DeepCopyEffectGroup(Outer, NewEffect);

	NewEffect->Condition = Condition;

	return NewEffect;
}

void UComplexEffect::ApplyEffect(FApplyEffectParams& Params) const
{
	check(Condition.Type == EConditionType::None || Params.Target);

	if (Condition.Type == EConditionType::None || Condition.IsActive(*Params.Target))
	{
		Super::ApplyEffect(Params);
	}
}
