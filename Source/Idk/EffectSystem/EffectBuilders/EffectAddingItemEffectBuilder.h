// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectAddingItemEffect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>

enum class EAddEffectLocation : uint8;
class UMultiStageBonusEffect;

/** Builder for effect-adding item effects. */
class FEffectAddingItemEffectBuilder final : public TEffectBuilder<UEffectAddingItemEffect,  FEffectAddingItemEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UEffectAddingItemEffect, FEffectAddingItemEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Initialize the effect-adding item effect.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Location			Where to add the effect. 
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UMultiStageBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FEffectAddingItemEffectBuilder& Init(const EAddEffectLocation Location, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UMultiStageBonusEffect, InEffectClass>, bool>>
inline FEffectAddingItemEffectBuilder& FEffectAddingItemEffectBuilder::Init(const EAddEffectLocation Location, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	Effect->LocationToAddEffect = Location;

	const FString EffectName = FString::Printf(TEXT("%s_EffectToAdd"), *Effect->GetFName().ToString());

	InEffectClass& EffectToAdd = Builder.Complete(EffectName);

	Effect->Effect = &EffectToAdd;

	return *this;
}
