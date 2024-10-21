// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/AbilityInfo.h"
#include <HAL/Platform.h>

class FMultiStageEffectBuilder;
class FName;
class UMultiStageEffect;
struct FAbilityTargetingInfo;
struct FGenericDisplayInfo;

template<class EffectClass, class BuilderType>
class TEffectBuilderDeferred;

/** Used to build abilities. */
class FAbilityInfoBuilder final
{
public:
	/**
	 * Begin the building process.
	 * 
	 * @return	An ability builder instance.
	 */
	UE_NODISCARD static FAbilityInfoBuilder Begin();

	/**
	 * Initialize the ability info.
	 * 
	 * @param Name			Unique name for the ability.
	 * @param TargetingInfo	Targeting info used by the ability. 
	 * @param Cooldown		Cooldown of the ability.	
	 * @param Duration		Duration of the ability.
	 * @param HitRate		For abilities that hit multiple times, the number of hits per second.
	 * @return				A reference to the ability builder.
	 */
	UE_NODISCARD FAbilityInfoBuilder&& Init(FName&& Name, FAbilityTargetingInfo&& TargetingInfo, const double Cooldown = 0.0, const double Duration = 0.0, const double HitRate = 0.0)&&;

	/**
	 * Set the effect used by the ability.
	 * 
	 * @param Builder	Builder used to build the effect used by the ability when activated. 
	 * @return			A reference to the ability builder.
	 */
	UE_NODISCARD FAbilityInfoBuilder&& SetEffect(TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder>&& Builder)&&;

	/**
	 * Set the information used to display the ability to the player.
	 * @param DisplayInfo	Information used to display the ability to the player.
	 * @return				A reference to the ability builder.
	 */
	UE_NODISCARD FAbilityInfoBuilder&& SetDisplayInfo(FGenericDisplayInfo&& DisplayInfo)&&;

	/**
	 * End the building process.
	 * 
	 * @return	Completed ability. 
	 */
	UE_NODISCARD FAbilityInfo&& Complete()&&;

private:
	/** Info about the ability being built. */
	FAbilityInfo AbilityInfo;

};
