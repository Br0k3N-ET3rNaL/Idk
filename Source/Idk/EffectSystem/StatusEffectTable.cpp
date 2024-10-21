// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/StatusEffectTable.h"

#include "Idk/EffectSystem/StatusEffectBlueprintBase.h"
#include "Idk/UI/KeywordTable.h"
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>
#include <Misc/Optional.h>
#include <Templates/SubclassOf.h>
#include <UObject/UObjectGlobals.h>

void FStatusEffectTooltipInfo::InitDescription(UAttributeSystemComponent& AttributeSystem) const
{
	StatusEffectClass.GetDefaultObject()->InitDescriptions(AttributeSystem);
}

FString FStatusEffectTooltipInfo::GetStatusEffectName() const
{
	return StatusEffectClass.GetDefaultObject()->GetName().ToString();
}

FKeywordInfo FStatusEffectTooltipInfo::GetKeywordInfo() const
{
	return FKeywordInfo(StatusEffectClass.GetDefaultObject()->GetDisplayInfo().GetDescription(), TextHighlightColor);
}

TSubclassOf<UStatusEffectBlueprintBase> FStatusEffectTooltipInfo::GetClass() const
{
	return StatusEffectClass;
}

bool FStatusEffectTooltipInfo::IsStatusEffectClassSet() const
{
	return StatusEffectClass != nullptr;
}

#if WITH_EDITOR
EDataValidationResult UStatusEffectTable::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	for (const TPair<FString, FStatusEffectTooltipInfo>& Pair : StatusEffects)
	{
		if (!Pair.Value.IsStatusEffectClassSet())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Class not set for %s."), *Pair.Key)));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			const FString& StatusEffectName = Pair.Value.GetStatusEffectName();

			if (Pair.Key != StatusEffectName)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Key (%s) doesn't match status effect name(%s)."), *Pair.Key, *StatusEffectName)));

				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif

void UStatusEffectTable::InitDescriptions(UAttributeSystemComponent& AttributeSystem) const
{
	for (const TPair<FString, FStatusEffectTooltipInfo>& Pair : StatusEffects)
	{
		Pair.Value.InitDescription(AttributeSystem);
	}
}

TOptional<FKeywordInfo> UStatusEffectTable::GetKeywordInfo(const FString& StatusEffectName) const
{
	if (const FStatusEffectTooltipInfo* TooltipInfo = StatusEffects.Find(StatusEffectName))
	{
		return TooltipInfo->GetKeywordInfo();
	}

	return TOptional<FKeywordInfo>();
}

TSubclassOf<UStatusEffectBlueprintBase> UStatusEffectTable::GetStatusEffectClassByName(const FString& StatusEffectName) const
{
	return StatusEffects[StatusEffectName].GetClass();
}
