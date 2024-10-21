// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include <HAL/Platform.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>

#include "ItemEffect.generated.h"

class UAttributeSystemComponent;

/** Base class for effects used by items and status effects. */
UCLASS(Abstract)
class UItemEffect : public UEffect
{
	GENERATED_BODY()

public:
	/**
	 * Apply the item effect to the target.
	 *
	 * @param Target	Attribute system to apply the effect to. 
	 */
	virtual void ApplyEffect(UAttributeSystemComponent* Target) const PURE_VIRTUAL(UItemEffect::ApplyEffect, ;);

	/**
	 * Remove the item effect from the target.
	 * 
	 * @param Target	Attribute system to apply the effect to.  
	 * @param Stacks	Number of stacks to remove. 
	 */
	virtual void RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks = 1) const PURE_VIRTUAL(UItemEffect::RemoveEffect, ;);

};
