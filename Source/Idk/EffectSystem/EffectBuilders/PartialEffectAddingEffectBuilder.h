// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/PartialEffectAddingItemEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>

enum class EAddEffectLocation : uint8;
enum class EEffectStage : uint8;
enum class EEffectType : uint8;
class UBonusEffect;

/** Builder for partial effect adding item effects. */
class FPartialEffectAddingEffectBuilder final : public TEffectBuilder<UPartialEffectAddingItemEffect, FPartialEffectAddingEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UPartialEffectAddingItemEffect, FPartialEffectAddingEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Initialize the effect.
	 * 
	 * @param Location				Where to add the partial effect to. 
	 * @param Stage					Which stage to add the partial effect to. Will be connected to effects in the previous stage.
	 * @param TypeToAddEffectTo		Type of effects to connect to the partial effect.
	 * @return						The builder.
	 */
	UE_NODISCARD FPartialEffectAddingEffectBuilder& Init(const EAddEffectLocation Location, const EEffectStage Stage, const EEffectType TypeToAddEffectTo);

	/**
	 * Set the bonus effect to add.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FPartialEffectAddingEffectBuilder& SetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UBonusEffect, InEffectClass>, bool>>
inline FPartialEffectAddingEffectBuilder& FPartialEffectAddingEffectBuilder::SetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	const FString EffectName = FString::Printf(TEXT("%s_EffectToAdd"), *Effect->GetFName().ToString());

	InEffectClass& EffectToAdd = Builder.Complete(EffectName);

	Effect->Info.EffectToAdd = &EffectToAdd;

	return *this;
}
