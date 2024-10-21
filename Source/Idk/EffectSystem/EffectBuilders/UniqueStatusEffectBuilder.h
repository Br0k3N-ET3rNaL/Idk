// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectBuilders/CompositeItemEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SingleStageEffectGroupBuilder.h"
#include "Idk/EffectSystem/StatusEffect.h"
#include "Idk/EffectSystem/StatusEffectInternal.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/Optional.h>
#include <type_traits>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

class UAttributeAlteringEffect;
class UItemEffect;
class USingleStageEffect;
struct FEffectId;
struct FGenericDisplayInfo;

/** Builder for unique status effects. */
class FUniqueStatusEffectBuilder final : public TEffectBuilder<UStatusEffect, FUniqueStatusEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<UStatusEffect, FUniqueStatusEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Set the information used to display the status effect to the player.
	 * 
	 * @param Name			Name of the status effect. 
	 * @param DisplayInfo	Information used to display the status effect to the player. @see FGenericDisplayInfo
	 * @return				The builder.
	 */
	UE_NODISCARD FUniqueStatusEffectBuilder& SetDisplayInfo(const FName& Name, FGenericDisplayInfo&& DisplayInfo);

	/**
	 * Set the status effect's information.
	 * 
	 * @param Duration		Duration of the status effect. 
	 * @param MaxStacks		Maximum number of stacks the status effect can have.
	 * @param bRefreshable	Whether the status effect is refreshed when re-applying the status effect.
	 * @return				The builder.
	 */
	UE_NODISCARD FUniqueStatusEffectBuilder& SetInfo(const double Duration, const uint8 MaxStacks = 1, const bool bRefreshable = false);

	/**
	 * Set the status effect's information. For permanent status effects.
	 * 
	 * @param MaxStacks	Maximum number of stacks the status effect can have.
	 * @return			The builder.
	 */
	UE_NODISCARD FUniqueStatusEffectBuilder& SetInfoPermanent(const uint8 MaxStacks = 1);

	/**
	 * Add an effect that will be applied to the target at regular intervals. 
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FUniqueStatusEffectBuilder& AddRepeatedEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

	/**
	 * Add an effect that will be applied/removed when the status effect is.
	 * 
	 * @tparam InEffectClass	Class of the effect to add. 
	 * @tparam InBuilderType	Type of the builder used to build the effect.
	 * @param Builder			Builder used to build the effect to add.
	 * @return					The builder for this effect.
	 */
	template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, InEffectClass>, bool> = true>
	UE_NODISCARD FUniqueStatusEffectBuilder& AddPersistentEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder);

private:
	/** Called during begin to perform setup. */
	void PostBegin()&;

	/** Called during complete for additional setup. */
	void PreComplete()&;

	/** Effect builder used to build the status effect's repeated effects. */
	TOptional<FSingleStageEffectGroupBuilder> RepeatedEffectsBuilder;

	/** Effect builder used to build the status effect's persistent effects. */
	TOptional<FCompositeItemEffectBuilder> PersistentEffectsBuilder;

};

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<USingleStageEffect, InEffectClass>, bool>>
inline FUniqueStatusEffectBuilder& FUniqueStatusEffectBuilder::AddRepeatedEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	if (!RepeatedEffectsBuilder)
	{
		const FString EffectName = FString::Printf(TEXT("%s_RepeatEffects"), *Effect->StatusEffect->GetFName().ToString());

		if (Effect->HasAllFlags(RF_DefaultSubObject))
		{
			RepeatedEffectsBuilder = FSingleStageEffectGroupBuilder::BeginDefault(Effect->GetOuter(), EffectName);
		}
		else
		{
			RepeatedEffectsBuilder = FSingleStageEffectGroupBuilder::Begin(Effect->GetOuter(), EffectName);
		}
	}

	RepeatedEffectsBuilder->AddEffect(MoveTemp(Builder));

	return *this;
}

template<class InEffectClass, class InBuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, InEffectClass>, bool>>
inline FUniqueStatusEffectBuilder& FUniqueStatusEffectBuilder::AddPersistentEffect(TEffectBuilderDeferred<InEffectClass, InBuilderType>&& Builder)
{
	if (!PersistentEffectsBuilder)
	{
		const FString EffectName = FString::Printf(TEXT("%s_PersistEffects"), *Effect->StatusEffect->GetFName().ToString());

		if (Effect->HasAllFlags(RF_DefaultSubObject))
		{
			PersistentEffectsBuilder = FCompositeItemEffectBuilder::BeginDefault(Effect->GetOuter(), EffectName);
		}
		else
		{
			PersistentEffectsBuilder = FCompositeItemEffectBuilder::Begin(Effect->GetOuter(), EffectName);
		}
	}

	PersistentEffectsBuilder->AddEffect(MoveTemp(Builder));

	return *this;
}
