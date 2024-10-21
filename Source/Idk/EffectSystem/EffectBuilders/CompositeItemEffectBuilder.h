// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/CompositeItemEffect.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>
#include <UObject/NameTypes.h>

class UItemEffect;

/** Builder for composite item effects. */
class FCompositeItemEffectBuilder final : public TEffectBuilder<UCompositeItemEffect, FCompositeItemEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UCompositeItemEffect, FCompositeItemEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Add an item effect to the composite item effect.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, InEffectClass>, bool> = true>
	FCompositeItemEffectBuilder& AddEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

private:
	/** Maps class names to the number of added effects of that class. */
	TMap<FName, int32> NumEffectsOfClass;

};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, InEffectClass>, bool>>
inline FCompositeItemEffectBuilder& FCompositeItemEffectBuilder::AddEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	const FName& ClassName = InEffectClass::StaticClass()->GetFName();

	int32& NumEffects = NumEffectsOfClass.FindOrAdd(ClassName);

	const FString EffectName = FString::Printf(TEXT("%s_%s_%i"), *Effect->GetFName().ToString(), *UEffect::GetEffectClassName<InEffectClass>(), NumEffects++);

	InEffectClass& EffectToAdd = Builder.Complete(EffectName);

	Effect->Effects.Add(&EffectToAdd);

	return *this;
}
