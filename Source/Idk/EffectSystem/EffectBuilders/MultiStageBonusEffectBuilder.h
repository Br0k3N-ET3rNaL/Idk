// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/MultiStageBonusEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>

class UBonusEffect;

/** Builder for multistage bonus effects. */
class FMultiStageBonusEffectBuilder final : public TEffectBuilder<UMultiStageBonusEffect, FMultiStageBonusEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UMultiStageBonusEffect, FMultiStageBonusEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Set the bonus effect applied to self.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageBonusEffectBuilder& SetSelfEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Set the bonus effect applied to the target.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageBonusEffectBuilder& SetTargetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Set the bonus effect applied to self after target effects.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageBonusEffectBuilder& SetCallbackEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

private:
	/**
	 * Set the bonus effect for the specified stage.
	 *
	 * @tparam InEffectClass	Class of the effect to add.
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Stage				Stage to add the effect to.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageBonusEffectBuilder& SetStageEffect(const EEffectStage Stage, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/** Number of bonus effects added. */
	int32 NumBonusEffects = 0;

};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool>>
inline FMultiStageBonusEffectBuilder& FMultiStageBonusEffectBuilder::SetSelfEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return SetStageEffect(EEffectStage::Self, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool>>
inline FMultiStageBonusEffectBuilder& FMultiStageBonusEffectBuilder::SetTargetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return SetStageEffect(EEffectStage::Target, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool>>
inline FMultiStageBonusEffectBuilder& FMultiStageBonusEffectBuilder::SetCallbackEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return SetStageEffect(EEffectStage::Callback, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_same_v<UBonusEffect, InEffectClass>, bool>>
inline FMultiStageBonusEffectBuilder& FMultiStageBonusEffectBuilder::SetStageEffect(const EEffectStage Stage, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	const FString EffectName = FString::Printf(TEXT("%s_BonusEffect_%i"), *Effect->GetFName().ToString(), NumBonusEffects++);

	UBonusEffect& EffectToAdd = Builder.Complete(EffectName);

	if (Effect->Effects.IsStageEmpty(Stage))
	{
		Effect->Effects.AddToStage(EffectToAdd, Stage);
	}
	else
	{
		Effect->Effects.GetStageArray(Stage)[0] = &EffectToAdd;
	}

	return *this;
}
