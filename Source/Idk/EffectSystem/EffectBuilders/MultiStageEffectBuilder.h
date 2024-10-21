// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>
#include <UObject/NameTypes.h>

class UObject;
class USingleStageEffect;

/** Builder for multistage effects. */
class FMultiStageEffectBuilder final : public TEffectBuilder<UMultiStageEffect, FMultiStageEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UMultiStageEffect, FMultiStageEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Add an effect that will be applied to self. 
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageEffectBuilder& AddSelfEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Add an effect that will be applied to the target.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageEffectBuilder& AddTargetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Add an effect that will be applied to self after target effects. 
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageEffectBuilder& AddCallbackEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Shortcut for creating basic damage effects. Should be used outside of constructors.
	 * 
	 * @param Outer		Object used to create the effect. 
	 * @param Damage	Amount of damage for the effect to deal. 
	 * @return			Builder used to finish building the effect.
	 */
	UE_NODISCARD static TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder> CreateBasicDamageEffect(UObject* Outer, const double Damage);

	/**
	 * Shortcut for creating basic damage effects. Should be used inside constructors.
	 *
	 * @param Outer		Object used to create the effect.
	 * @param Damage	Amount of damage for the effect to deal.
	 * @return			Builder used to finish building the effect.
	 */
	UE_NODISCARD static TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder> CreateBasicDamageEffectDefault(UObject* Outer, const double Damage);

private:
	/**
	 * Add a single stage effect to the specified stage.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Stage				Stage to add the effect to.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FMultiStageEffectBuilder& AddEffectToStage(const EEffectStage Stage, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/** Maps class names to the number of added effects of that class. */
	TMap<FName, int32> NumEffectsOfClass;
};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline FMultiStageEffectBuilder& FMultiStageEffectBuilder::AddSelfEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return AddEffectToStage(EEffectStage::Self, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline FMultiStageEffectBuilder& FMultiStageEffectBuilder::AddTargetEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return AddEffectToStage(EEffectStage::Target, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline FMultiStageEffectBuilder& FMultiStageEffectBuilder::AddCallbackEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	return AddEffectToStage(EEffectStage::Callback, MoveTemp(Builder));
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline FMultiStageEffectBuilder& FMultiStageEffectBuilder::AddEffectToStage(const EEffectStage Stage, TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	const FName& EffectClassName = InEffectClass::StaticClass()->GetFName();

	int32& NumEffects = NumEffectsOfClass.FindOrAdd(EffectClassName);

	const FString EffectName = FString::Printf(TEXT("%s_%s_%i"), *Effect->GetFName().ToString(), *UEffect::GetEffectClassName<InEffectClass>(), NumEffects++);

	InEffectClass& InEffect = Builder.Complete(EffectName);

	Effect->Effects.AddToStage(InEffect, Stage);

	return *this;
}
