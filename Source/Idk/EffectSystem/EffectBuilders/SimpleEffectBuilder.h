// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SimpleEffect.h"
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

class USingleStageEffect;
struct FEffectVariableValueInfo;

/** Builder for simple effects. */
class FSimpleEffectBuilder final : public TEffectBuilder<USimpleEffect, FSimpleEffectBuilder>
{
	friend class TEffectBuilder;

	using TEffectBuilder<USimpleEffect, FSimpleEffectBuilder>::TEffectBuilder;

public:
	/**
	 * Initialize a damage effect.
	 * 
	 * @param Damage		Amount of damage to deal. 
	 * @param bUseModifiers Whether the damage should use modifiers.
	 * @return				The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitDamage(const double Damage, const bool bUseModifiers = true);

	/**
	 * Initialize a heal effect.
	 * 
	 * @param HealAmount	Amount of health to recover. 
	 * @param bUseModifiers Whether the heal should use modifiers.
	 * @return				The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitHeal(const double HealAmount, const bool bUseModifiers = true);

	/**
	 * Set the ID to save the effect's value to. Should only be used for damage and heal effects.
	 * 
	 * @param SaveId	ID to save the effect's value to. 
	 * @return			The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& SetSaveId(const FEffectId SaveId);

	/**
	 * Add a variable value that the effect should use.
	 * 
	 * @param UseId				ID of the variable value to use.	 
	 * @param VariableValueInfo	Information about how the variable value should be used. @see FEffectVariableValueInfo 
	 * @return 
	 */
	UE_NODISCARD FSimpleEffectBuilder& AddValueToUse(const FEffectId UseId, FEffectVariableValueInfo&& VariableValueInfo);

	/**
	 * Initialize an effect that will save the amount of health the target is missing.
	 * 
	 * @param SaveId	ID to save the amount of missing health to. 
	 * @return			The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitGetMissingHealth(const FEffectId SaveId);

	/**
	 * Initialize an effect that will save the target's current health percentage.
	 * 
	 * @param SaveId	ID to save the health percentage to.
	 * @return			The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitGetHealthPercent(const FEffectId SaveId);

	/**
	 * Initialize an effect that will save the number of stacks of a status effect on the target.
	 * 
	 * @param SaveId					ID to save the status effect stacks to.
	 * @param StatusEffectToCheckFor	Name of the status effect to get the stacks for.
	 * @return							The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitGetStatusEffectStacks(const FEffectId SaveId, const FName StatusEffectToCheckFor);

	/**
	 * Initialize an effect that will remove a status effect from the target.
	 * 
	 * @param StatusEffectToCheckFor	Name of the status effect to remove. If not set, will remove all status effects. 
	 * @return							The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitRemoveStatusEffects(const FName StatusEffectToCheckFor = FName());

	/**
	 * Initializes an effect that will disable an item on the target.
	 * 
	 * @param ItemName	Name of the item to disable. 
	 * @return			The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitDisableItem(const FName& ItemName);

	/**
	 * Initializes an effect that can push or pull the target.
	 * 
	 * @param Amount	Amount of force to apply to the target. Values in the range +/-[1.0,4.0] produce reasonable amounts of force.
	 * @return			The builder.
	 */
	UE_NODISCARD FSimpleEffectBuilder& InitPush(const double Amount);

};
