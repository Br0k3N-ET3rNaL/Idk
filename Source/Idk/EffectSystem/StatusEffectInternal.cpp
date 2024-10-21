// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/StatusEffectInternal.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/StatusEffectComponent.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/CompositeItemEffect.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectSavedValues.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/EffectSystem/SingleStageEffectGroup.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include "Idk/UI/TooltipFormatParam.h"
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>

const FString UStatusEffectInternal::EffectClassName = TEXT("StatusEffectIntern");

const FTextFormat UStatusEffectInternal::TooltipFormatText = FText::FromString(TEXT("<StatusEffect Desc=\"{1}\">{0}</>"));

#if WITH_EDITOR
void UStatusEffectInternal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName& PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UStatusEffectInternal, RepeatedEffects)))
	{
		if (RepeatedEffects)
		{
			RepeatedEffects->SetSelfContained();
		}
	}
}

EDataValidationResult UStatusEffectInternal::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (Name.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("UStatusEffectInternal: Name is not set")));

		Result = EDataValidationResult::Invalid;
	}

	FString InvalidReason;

	if (!DisplayInfo.IsValid(InvalidReason))
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("UStatusEffectInternal: Display info is invalid: %s"), *InvalidReason)));

		Result = EDataValidationResult::Invalid;
	}

	if (Duration != PermanentDuration && Duration <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("UStatusEffectInternal: Invalid duration")));

		Result = EDataValidationResult::Invalid;
	}

	if (RepeatedEffects)
	{
		TSet<FUseIdInfo> EffectUseIds = RepeatedEffects->GetUseIds();

		for (const FUseIdInfo& UseIdInfo : EffectUseIds)
		{
			if (!UseIdInfo.bUseLocalValue)
			{
				Context.AddError(FText::FromString(FString::Printf(
					TEXT("UStatusEffectInternal: bUseLocalValue for ID = %u was false, but effects within status effects can only use local values"),
					UseIdInfo.UseId)));

				Result = EDataValidationResult::Invalid;
			}
		}

		FDataValidationContext RepeatedEffectsContext;

		if (RepeatedEffects->IsEffectValid(RepeatedEffectsContext, EEffectValidationFlags::InStatusEffect) == EDataValidationResult::Invalid)
		{
			const FText InvalidEffectPrefix = FText::FromString(TEXT("UStatusEffectInternal: Repeated effect was invalid: "));

			FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, RepeatedEffectsContext);

			Result = EDataValidationResult::Invalid;
		}
	}

	FDataValidationContext PersistentEffectsContext;

	if (PersistentEffects && PersistentEffects->IsEffectValid(PersistentEffectsContext, EEffectValidationFlags::InStatusEffect) == EDataValidationResult::Invalid)
	{
		const FText InvalidEffectPrefix = FText::FromString(TEXT("UStatusEffectInternal: Persistent effect was invalid: "));

		FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, PersistentEffectsContext);

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

UEffect* UStatusEffectInternal::DuplicateEffectInternal(UObject* Outer, const FName& InName) const
{
	UStatusEffectInternal* NewEffect = DuplicateBaseEffect<UStatusEffectInternal>(Outer, InName);

	NewEffect->Name = Name;
	NewEffect->DisplayInfo = DisplayInfo;
	NewEffect->Duration = Duration;
	NewEffect->MaxStacks = MaxStacks;
	NewEffect->bRefreshable = bRefreshable;

	if (RepeatedEffects)
	{
		NewEffect->RepeatedEffects = UEffect::DuplicateEffect(*RepeatedEffects, Outer, *this, *NewEffect);
	}

	if (PersistentEffects)
	{
		NewEffect->PersistentEffects = UEffect::DuplicateEffect(*PersistentEffects, Outer, *this, *NewEffect);
	}

	return NewEffect;
}

void UStatusEffectInternal::InitDescriptions(UAttributeSystemComponent& AttributeSystem)
{
	if (RepeatedEffects)
	{
		RepeatedEffects->InitDescriptions(AttributeSystem);
	}

	DisplayInfo.GetDescriptionFormatArgsDelegate.BindUObject(this, &UStatusEffectInternal::GetTooltipParams, &AttributeSystem);

	DisplayInfo.InitDescription(AttributeSystem);
}

void UStatusEffectInternal::ApplyEffect(FApplyEffectParams& Params) const
{
	Params.Target->ApplyStatusEffect(*this);
}

FText UStatusEffectInternal::GetTooltipText() const
{
	return FText::FormatOrdered(TooltipFormatText, DisplayInfo.GetDisplayName(), DisplayInfo.GetDescription());
}

void UStatusEffectInternal::ActivateEffect(UAttributeSystemComponent* Target, const uint8 Stacks, const double Interval) const
{
	if (RepeatedEffects)
	{
		FEffectSavedValues SavedValues;

		SavedValues.SaveValue(FEffectId::StackId, Stacks);

		FApplyEffectParams Params(nullptr, Target, Interval, false, ETriggerEffects::None, nullptr, &SavedValues);

		RepeatedEffects->ApplyEffect(Params);
	}
}

void UStatusEffectInternal::ApplyPersistentEffects(UAttributeSystemComponent* Target) const
{
	if (PersistentEffects)
	{
		PersistentEffects->ApplyEffect(Target);
	}
}

void UStatusEffectInternal::RemoveAttributeAlteringEffects(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	if (PersistentEffects) 
	{
		PersistentEffects->RemoveEffect(Target, Stacks);
	}
}

const FName& UStatusEffectInternal::GetName() const
{
	return Name;
}

const FGenericDisplayInfo& UStatusEffectInternal::GetDisplayInfo() const
{
	return DisplayInfo;
}

void UStatusEffectInternal::GetTooltipParams(FFormatNamedArguments& InOutArgs, UAttributeSystemComponent* Self) const
{
	TMap<FName, FText> StatusEffectParams;
	TMap<EEffectType, double> EffectValueParams;

	for (TPair<FString, FFormatArgumentValue>& Pair : InOutArgs)
	{
		if (FTooltipFormatParam::IsSingleStageEffectValue(Pair.Key))
		{
			EffectValueParams.Add(FTooltipFormatParam::GetEffectType(Pair.Key));
		}
		else if (FTooltipFormatParam::IsDuration(Pair.Key))
		{
			Pair.Value = FText::AsNumber(Duration);
		}
		else if (FTooltipFormatParam::IsInterval(Pair.Key))
		{
			Pair.Value = FText::AsNumber(UStatusEffectComponent::TimerInterval);
		}
	}

	if (RepeatedEffects)
	{
		FApplyEffectParams EffectParams = FApplyEffectParams(Self, nullptr, UStatusEffectComponent::TimerInterval / Duration, true);

		RepeatedEffects->GetTooltipParams(EffectParams, StatusEffectParams, EffectValueParams);
	}

	for (const TPair<EEffectType, double>& Pair : EffectValueParams)
	{
		InOutArgs[FTooltipFormatParam::FromSingleStageEffectValue(Pair.Key)] = FText::AsNumber(Pair.Value);
	}
}

bool UStatusEffectInternal::HasPersistentEffects() const
{
	return PersistentEffects != nullptr;
}
