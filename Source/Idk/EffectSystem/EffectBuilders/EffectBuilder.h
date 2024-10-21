// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UnrealTemplate.h>
#include <type_traits>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>

/**
 * Class used to defer the final step when building an effect.
 *
 * @tparam EffectClass	Class of the effect being built.
 * @tparam BuilderType	Specific effect builder subclass.
 */
template<class EffectClass, class BuilderType>
class TEffectBuilderDeferred final
{
	template<class EffectClass, class BuilderType>
	friend class TEffectBuilder;

public:
	~TEffectBuilderDeferred();

	/**
	 * Finish building the effect.
	 *
	 * @param Name	Name of the effect to create.
	 * @return		The completed effect.
	 */
	EffectClass& Complete(const FString& Name);

private:
	TEffectBuilderDeferred(EffectClass* TemplateEffect);

	/** Effect used as a template to create the final effect. */
	EffectClass* TemplateEffect;
};

/**
 * Base class for effect builders.
 * 
 * @tparam EffectClass	Class of the effect to build. 
 * @tparam BuilderType	Specific effect builder subclass.
 */
template<class EffectClass, class BuilderType>
class TEffectBuilder
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	template<class EffectClass, class BuilderType>
	friend class TEffectBuilderDeferred;

public:
	~TEffectBuilder();

	/**
	 * Start building the effect. Should be used outside constructors.
	 * 
	 * @param Outer	Object used to create the effect. 
	 * @return		The builder.
	 */
	UE_NODISCARD static BuilderType Begin(UObject* Outer);

	/**
	 * Start building the effect. Should be used outside constructors.
	 * 
	 * @param Outer	Object used to create the effect.
	 * @param Name	Name of the effect to create.
	 * @return		The builder.
	 */
	UE_NODISCARD static BuilderType Begin(UObject* Outer, const FString& Name);

	/**
	 * Start building the effect. Should be used inside constructors.
	 * 
	 * @param Outer	Object used to create the effect. 
	 * @return		The builder.
	 */
	UE_NODISCARD static BuilderType BeginDefault(UObject* Outer);

	/**
	 * Start building the effect. Should be used inside constructors.
	 * 
	 * @param Outer	Object used to create the effect.
	 * @param Name	Name of the effect to create.
	 * @return		The builder.
	 */
	UE_NODISCARD static BuilderType BeginDefault(UObject* Outer, const FString& Name);

	/**
	 * Finish building the effect.
	 * 
	 * @return	The completed effect. 
	 */
	UE_NODISCARD EffectClass& Complete();

	/**
	 * Finish building and validate the effect.
	 *
	 * @return	The completed effect.
	 */
	UE_NODISCARD EffectClass& CompleteWithValidation();

	/**
	 * Defer the final step of building.
	 * 
	 * @return	An instance of TEffectBuilderDeferred that can be used to finish building the effect. 
	 */
	UE_NODISCARD TEffectBuilderDeferred<EffectClass, BuilderType> DeferCompletion();

protected:
	TEffectBuilder(EffectClass* Effect);

	/** Effect being built. */
	EffectClass* Effect;

private:
	/** Called during begin for additional setup. */
	void PostBegin()&;

	/** Called during complete for additional setup. */
	void PreComplete()&;
	
};

template<class EffectClass, class BuilderType>
inline TEffectBuilderDeferred<EffectClass, BuilderType>::~TEffectBuilderDeferred()
{
	
}

template<class EffectClass, class BuilderType>
inline EffectClass& TEffectBuilderDeferred<EffectClass, BuilderType>::Complete(const FString& Name)
{
	EffectClass* FinalEffect = (TemplateEffect->GetFName().IsEqual(*Name))
		? TemplateEffect
		: UEffect::DuplicateEffect(*TemplateEffect, TemplateEffect->GetOuter(), *Name);

	TemplateEffect->RemoveFromRoot();
	TemplateEffect = nullptr;

	return *FinalEffect;
}

template<class EffectClass, class BuilderType>
inline TEffectBuilderDeferred<EffectClass, BuilderType>::TEffectBuilderDeferred(EffectClass* TemplateEffect)
	: TemplateEffect(TemplateEffect)
{
}

template<class EffectClass, class BuilderType>
inline TEffectBuilder<EffectClass, BuilderType>::~TEffectBuilder()
{
	if (Effect)
	{
		Effect->RemoveFromRoot();
		Effect = nullptr;
	}
}

template<class EffectClass, class BuilderType>
inline BuilderType TEffectBuilder<EffectClass, BuilderType>::Begin(UObject* Outer)
{
	BuilderType Builder = BuilderType(UEffect::CreateEffect<EffectClass>(Outer));

	Builder.PostBegin();

	return Builder;
}

template<class EffectClass, class BuilderType>
inline BuilderType TEffectBuilder<EffectClass, BuilderType>::Begin(UObject* Outer, const FString& Name)
{
	BuilderType Builder = BuilderType(UEffect::CreateEffect<EffectClass>(Outer, *Name));

	Builder.PostBegin();

	return Builder;
}

template<class EffectClass, class BuilderType>
inline BuilderType TEffectBuilder<EffectClass, BuilderType>::BeginDefault(UObject* Outer)
{
	BuilderType Builder = BuilderType(UEffect::CreateEffectDefault<EffectClass>(Outer));

	Builder.PostBegin();

	return Builder;
}

template<class EffectClass, class BuilderType>
inline BuilderType TEffectBuilder<EffectClass, BuilderType>::BeginDefault(UObject* Outer, const FString& Name)
{
	BuilderType Builder = BuilderType(UEffect::CreateEffectDefault<EffectClass>(Outer, *Name));

	Builder.PostBegin();

	return Builder;
}

template<class EffectClass, class BuilderType>
inline void TEffectBuilder<EffectClass, BuilderType>::PostBegin()&
{
}

template<class EffectClass, class BuilderType>
inline void TEffectBuilder<EffectClass, BuilderType>::PreComplete()&
{
}

template<class EffectClass, class BuilderType>
inline EffectClass& TEffectBuilder<EffectClass, BuilderType>::Complete()
{
	((BuilderType*)this)->PreComplete();

	EffectClass& BuiltEffect = *Effect;

	Effect->RemoveFromRoot();
	Effect = nullptr;

	return BuiltEffect;
}

template<class EffectClass, class BuilderType>
inline EffectClass& TEffectBuilder<EffectClass, BuilderType>::CompleteWithValidation()
{
	((BuilderType*)this)->PreComplete();

#if WITH_EDITOR
	Effect->CheckIsValidEffect(EEffectValidationFlags::InitialValidation);
#endif

	Effect->RemoveFromRoot();

	EffectClass& BuiltEffect = *Effect;

	Effect = nullptr;

	return BuiltEffect;
}

template<class EffectClass, class BuilderType>
inline TEffectBuilderDeferred<EffectClass, BuilderType> TEffectBuilder<EffectClass, BuilderType>::DeferCompletion()
{
	((BuilderType*)this)->PreComplete();

	EffectClass* BuiltEffect = Effect;
	Effect = nullptr;

	return TEffectBuilderDeferred<EffectClass, BuilderType>(BuiltEffect);
}

template<class EffectClass, class BuilderType>
inline TEffectBuilder<EffectClass, BuilderType>::TEffectBuilder(EffectClass* Effect)
	: Effect(Effect)
{
	check(Effect);

	// Ensure effect is not garbage collected while building
	Effect->AddToRoot();
}
