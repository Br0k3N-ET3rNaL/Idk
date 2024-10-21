// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/StatusEffectBlueprintBase.h"

#include "Idk/EffectSystem/StatusEffectInternal.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <HAL/Platform.h>
#include <Internationalization/Text.h>

const FText UStatusEffectBlueprintBase::TooltipFormatText = FText::FromString(TEXT("<StatusEffect id=\"{1}\">{0}</>"));

FText UStatusEffectBlueprintBase::GetTooltipText() const
{
	return FText::FormatOrdered(TooltipFormatText, DisplayInfo.GetDisplayName(), FText::FromName(Name));
}
