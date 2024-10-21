// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/ComplexEffectBuilder.h"

#include "Idk/EffectSystem/ComplexEffect.h"
#include "Idk/EffectSystem/ConditionComparator.h"
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

FComplexEffectBuilder& FComplexEffectBuilder::SetConditionToHealthPercentComparison(const double Comparand, const EConditionComparator Comparator)
{
	Effect->Condition.Type = EConditionType::HealthPercentComparison;
	Effect->Condition.Comparand = Comparand;
	Effect->Condition.Comparator = (uint8) Comparator;

	return *this;
}

FComplexEffectBuilder& FComplexEffectBuilder::SetConditionToStatusEffectExists(const FName& StatusEffectToCheckFor)
{
	Effect->Condition.Type = EConditionType::StatusEffectExists;
	Effect->Condition.StatusEffectToCheckFor = StatusEffectToCheckFor;

	return *this;
}
