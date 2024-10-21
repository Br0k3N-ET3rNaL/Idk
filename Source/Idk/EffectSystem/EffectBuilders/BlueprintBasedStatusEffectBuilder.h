// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/StatusEffect.h"
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>

class UStatusEffectBlueprintBase;

/** Builder for blueprint-based status effects. */
class FBlueprintBasedStatusEffectBuilder final : public TEffectBuilder<UStatusEffect, FBlueprintBasedStatusEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UStatusEffect, FBlueprintBasedStatusEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Initialize the blueprint based status effect.
	 * 
	 * @param StatusEffectClass	Blueprint class of the status effect. 
	 * @return					The builder.
	 */
	UE_NODISCARD FBlueprintBasedStatusEffectBuilder& Init(TSubclassOf<UStatusEffectBlueprintBase> StatusEffectClass);

};

