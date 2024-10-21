// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/AttributeAlteringEffect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include <HAL/Platform.h>

enum class EAttributeType : uint8;

/** Builder for attribute altering effects. */
class FAttributeAlteringEffectBuilder final : public TEffectBuilder<UAttributeAlteringEffect, FAttributeAlteringEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UAttributeAlteringEffect, FAttributeAlteringEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Initialize the attribute altering effect.
	 * 
	 * @param AttributeToAlter	Type of attribute to alter. 
	 * @param Bonus				Bonus to the attribute's base value.
	 * @param MultiplierBonus	Bonus to the attribute's multiplier.
	 * @return					The builder. 
	 */
	UE_NODISCARD FAttributeAlteringEffectBuilder& Init(const EAttributeType AttributeToAlter, const double Bonus = 0.0, const double MultiplierBonus = 0.0);

};
