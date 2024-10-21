// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/ComplexEffect.h"
#include "Idk/EffectSystem/ConditionComparator.h"
#include "Idk/EffectSystem/EffectBuilders/SingleStageEffectGroupBuilder.h"
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

/** Builder for complex effects. */
class FComplexEffectBuilder final : public TSingleStageEffectGroupBuilder<UComplexEffect, FComplexEffectBuilder>
{
	friend class TEffectBuilder;

	using TSingleStageEffectGroupBuilder<UComplexEffect, FComplexEffectBuilder>::TSingleStageEffectGroupBuilder;

public:
	/**
	 * Set the effect's condition to compare the character's health percentage with a specified value.
	 * 
	 * @param Comparand		Value to compare with health percentage (in the range [0.0, 1.0]). 
	 * @param Comparator	How to compare the health percentage with the comparand.
	 * @return				The builder.
	 */
	UE_NODISCARD FComplexEffectBuilder& SetConditionToHealthPercentComparison(const double Comparand, const EConditionComparator Comparator);

	/**
	 * Set the effect's condition to check if a specific status effect exists on the character.
	 * 
	 * @param StatusEffectToCheckFor	Name of the status effect to check for. 
	 * @return							The builder.
	 */
	UE_NODISCARD FComplexEffectBuilder& SetConditionToStatusEffectExists(const FName& StatusEffectToCheckFor);

};
