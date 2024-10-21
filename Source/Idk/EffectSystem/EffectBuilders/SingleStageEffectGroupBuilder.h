// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/EffectSystem/SingleStageEffectGroup.h"
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <type_traits>
#include <UObject/NameTypes.h>

/**
 * Base class for single stage effect group builders.
 *
 * @tparam EffectClass	Class of the effect to build.
 * @tparam BuilderType	Specific effect builder subclass.
 */
template<class EffectClass, class BuilderType>
class TSingleStageEffectGroupBuilder : public TEffectBuilder<EffectClass, BuilderType>
{
	static_assert(std::is_base_of<USingleStageEffectGroup, EffectClass>::value);

	using TEffectBuilder<EffectClass, BuilderType>::TEffectBuilder;

public:
	/**
	 * Add a single stage effect to the effect group.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	BuilderType& AddEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

private:
	/** Maps class names to the number of added effects of that class. */
	TMap<FName, int32> NumEffectsOfClass;

};

/** Builder for USingleStageEffectGroup. */
class FSingleStageEffectGroupBuilder : public TSingleStageEffectGroupBuilder<USingleStageEffectGroup, FSingleStageEffectGroupBuilder>
{
	friend class TEffectBuilder;

	using TSingleStageEffectGroupBuilder<USingleStageEffectGroup, FSingleStageEffectGroupBuilder>::TSingleStageEffectGroupBuilder;

};

template<class EffectClass, class BuilderType>
template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline BuilderType& TSingleStageEffectGroupBuilder<EffectClass, BuilderType>::AddEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	const FName& ClassName = InEffectClass::StaticClass()->GetFName();

	int32& NumEffects = NumEffectsOfClass.FindOrAdd(ClassName);

	const FString EffectName = FString::Printf(TEXT("%s_%s_%i"), *this->Effect->GetFName().ToString(), *UEffect::GetEffectClassName<InEffectClass>(), NumEffects++);

	InEffectClass& EffectToAdd = Builder.Complete(EffectName);

	this->Effect->Effects.Add(&EffectToAdd);

	return *(BuilderType*)this;
}
