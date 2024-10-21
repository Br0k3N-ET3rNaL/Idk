// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectId.h"
#include <Containers/Map.h>
#include <HAL/Platform.h>

/** Container that can be used to save and retrieve values based on IDs. */
struct FEffectSavedValues final
{
public:
	/**
	 * @param bFromMultiHit	Whether the value comes from an effect that is applied multiple times (status effect or multi-hit ability). 
	 */
	FEffectSavedValues(const bool bFromMultiHit = true);

	/**
	 * Save a value.
	 * 
	 * @param SaveId	ID used to save the value. 
	 * @param Value		The value to save.
	 */
	void SaveValue(const FEffectId SaveId, const double Value);

	/** Get the saved value for the specified ID. */
	UE_NODISCARD double GetSavedValue(const FEffectId SaveId) const;

	/** Check whether any values have been saved. */
	UE_NODISCARD bool HasSavedValues() const;

	/** Check whether the value comes from an effect that is applied multiple times (status effect or multi-hit ability). */
	UE_NODISCARD bool IsFromMultiHitEffect() const;

private:
	/** Maps effect IDs to saved values. */
	TMap<FEffectId, double> SavedValues;

	/** Whether the value comes from an effect that is applied multiple times (status effect or multi-hit ability). */
	bool bFromMultiHit;
};