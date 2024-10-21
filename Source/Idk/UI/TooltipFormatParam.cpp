// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/UI/TooltipFormatParam.h"

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectStage.h"
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

const FString FTooltipFormatParam::StatusEffectPrefix = TEXT("StatusEffect=");
const FString FTooltipFormatParam::DurationString = TEXT("Duration");
const FString FTooltipFormatParam::IntervalString = TEXT("Interval");

const TMap<EEffectStage, FString> FTooltipFormatParam::EffectStageStrings = { 
	{EEffectStage::Self, TEXT("Self")},
	{EEffectStage::Target, TEXT("Target")},
	{EEffectStage::Callback, TEXT("Callback")} 
};

const TMap<EEffectType, FString> FTooltipFormatParam::EffectTypeStrings = { 
	{EEffectType::Damage, TEXT("Damage")}, 
	{EEffectType::Healing, TEXT("Healing")} 
};

bool FTooltipFormatParam::IsMultiStageEffectValue(const FString& Param)
{
	bool bHasStage = false;

	for (const TPair<EEffectStage, FString> EffectStageString : EffectStageStrings)
	{
		if (Param.StartsWith(EffectStageString.Value))
		{
			bHasStage = true;
			break;
		}
	}

	if (!bHasStage)
	{
		return false;
	}

	for (const TPair<EEffectType, FString> EffectTypeString : EffectTypeStrings)
	{
		if (Param.EndsWith(EffectTypeString.Value))
		{
			return true;
		}
	}

	return false;
}

bool FTooltipFormatParam::IsSingleStageEffectValue(const FString& Param)
{
	for (const TPair<EEffectType, FString> EffectTypeString : EffectTypeStrings)
	{
		if (Param.Equals(EffectTypeString.Value))
		{
			return true;
		}
	}

	return false;
}

bool FTooltipFormatParam::IsStatusEffect(const FString& Param)
{
	return Param.StartsWith(StatusEffectPrefix);
}

bool FTooltipFormatParam::IsDuration(const FString& Param)
{
	return Param.Equals(DurationString);
}

bool FTooltipFormatParam::IsInterval(const FString& Param)
{
	return Param.Equals(IntervalString);
}

EEffectStage FTooltipFormatParam::GetEffectStage(const FString& Param)
{
	for (const TPair<EEffectStage, FString> EffectStageString : EffectStageStrings)
	{
		if (Param.StartsWith(EffectStageString.Value))
		{
			return EffectStageString.Key;
		}
	}

	return EEffectStage::None;
}

EEffectType FTooltipFormatParam::GetEffectType(const FString& Param)
{
	for (const TPair<EEffectType, FString> EffectTypeString : EffectTypeStrings)
	{
		if (Param.EndsWith(EffectTypeString.Value))
		{
			return EffectTypeString.Key;
		}
	}

	return EEffectType::None;
}

FName FTooltipFormatParam::GetStatusEffectName(const FString& Param)
{
	if (IsStatusEffect(Param))
	{
		FString Temp = Param;
		Temp.RemoveFromStart(StatusEffectPrefix);

		return FName(*Temp);
	}

	return FName();
}

FString FTooltipFormatParam::FromStatusEffect(const FName& StatusEffectName)
{
	return StatusEffectPrefix + StatusEffectName.ToString();
}

FString FTooltipFormatParam::FromMultiStageEffectValue(const EEffectStage Stage, const EEffectType EffectType)
{
	return EffectStageStrings[Stage] + EffectTypeStrings[EffectType];
}

FString FTooltipFormatParam::FromSingleStageEffectValue(const EEffectType EffectType)
{
	return EffectTypeStrings[EffectType];
}
