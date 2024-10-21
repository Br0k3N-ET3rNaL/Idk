// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/Crc.h>
#include <Templates/Function.h>
#include <UObject/ObjectMacros.h>
#include <Windows/WindowsPlatform.h>

#include "SingleStageEffect.generated.h"

class AIdkCharacter;
class FName;
class FText;
class UAttributeSystemComponent;
class USimpleEffect;
class USingleStageEffect;
struct FEffectSavedValues;
struct FSimpleEffectTemp;
struct FTooltipFormatParam;

/** Represents what kind of effects should be triggered in response to an effect being applied to a target. */
UENUM()
enum class ETriggerEffects : uint8
{
	None,
	Friendly,
	Hostile,
};

/** Information used when applying a single stage effect to a target. */
struct FApplyEffectParams final
{
public:
	/**
	 * @param Applier				Attribute system of the character who applied the effect.
	 * @param Target				Attribute system of the character who the effect is being applied to.
	 * @param EffectOrigin			Where the effect is being applied from.
	 * @param PerHitMultiplier		Multiplier applied to effects which are applied multiple times. @note Used by status effects and abilities with multiple hits.
	 * @param bUseModifiers			Whether the effect should apply modifiers to effect values.
	 * @param EffectsToTrigger		Which effects should be triggered when applying the effect.
	 * @param PreviousSavedValues	Values saved by effects in previous stages.
	 * @param CurrentSavedValues	Values saved by effects in the current stage. 
	 */
	FApplyEffectParams(
		UAttributeSystemComponent* Applier,
		UAttributeSystemComponent* Target,
		const FVector2D EffectOrigin,
		const double PerHitMultiplier = 1.0,
		const bool bUseModifiers = false,
		const ETriggerEffects EffectsToTrigger = ETriggerEffects::None,
		const FEffectSavedValues* PreviousSavedValues = nullptr,
		FEffectSavedValues* CurrentSavedValues = nullptr
	);

	/**
	 * @param Applier				Attribute system of the character who applied the effect. 
	 * @param Target				Attribute system of the character who the effect is being applied to.
	 * @param PerHitMultiplier		Multiplier applied to effects which are applied multiple times. @note Used by status effects and abilities with multiple hits.
	 * @param bUseModifiers			Whether the effect should apply modifiers to effect values.
	 * @param EffectsToTrigger		Which effects should be triggered when applying the effect.
	 * @param PreviousSavedValues	Values saved by effects in previous stages.
	 * @param CurrentSavedValues	Values saved by effects in the current stage. 
	 */
	FApplyEffectParams(
		UAttributeSystemComponent* Applier,
		UAttributeSystemComponent* Target,
		const double PerHitMultiplier = 1.0,
		const bool bUseModifiers = false,
		const ETriggerEffects EffectsToTrigger = ETriggerEffects::None,
		const FEffectSavedValues* PreviousSavedValues = nullptr,
		FEffectSavedValues* CurrentSavedValues = nullptr
	);

	/** Attribute system of the character who applied the effect. */
	UAttributeSystemComponent* Applier;

	/** Attribute system of the character who the effect is being applied to. */
	UAttributeSystemComponent* Target;

	/** Where the effect is being applied from. */
	const FVector2D EffectOrigin;

	/** Multiplier applied to effects which are applied multiple times. @note Used by status effects and abilities with multiple hits. */
	const double PerHitMultiplier;

	/** Whether the effect should apply modifiers to effect values. */
	const bool bUseModifiers;

	/** Which effects should be triggered when applying the effect. @see ETriggerEffects */
	const ETriggerEffects EffectsToTrigger;

	/** Values saved by effects in previous stages. */
	const FEffectSavedValues* PreviousSavedValues;

	/** Values saved by effects in the current stage. */
	FEffectSavedValues* CurrentSavedValues;

	/** Whether the effect origin was set. */
	bool bEffectOriginSet = false;
};

/** Information representing a variable value used by an effect. */
struct FUseIdInfo final
{
public:
	/**
	 * @param UseId				ID of the value to use. 
	 * @param bUseLocalValue	Whether the value comes from the current stage or the previous stage. 
	 */
	FUseIdInfo(const FEffectId UseId, const bool bUseLocalValue);

	UE_NODISCARD friend bool operator==(const FUseIdInfo& Lhs, const FUseIdInfo& Rhs)
	{
		return Lhs.UseId == Rhs.UseId && Lhs.bUseLocalValue == Rhs.bUseLocalValue;
	}

	/** ID of the value to use. */
	const FEffectId UseId;

	/** Whether the value comes from the current stage or the previous stage. */
	const bool bUseLocalValue;
};

FORCEINLINE uint32 GetTypeHash(const FUseIdInfo& UseIdInfo)
{
	return FCrc::MemCrc32(&UseIdInfo, sizeof(FUseIdInfo));
}

/** Base class for instantaneous effects that can be applied to a target. */
UCLASS(Abstract)
class USingleStageEffect : public UEffect
{
	GENERATED_BODY()

public:
	/** Initialize effect descriptions based on modifiers from the specified attribute system. */
	virtual void InitDescriptions(UAttributeSystemComponent& AttributeSystem) PURE_VIRTUAL(USingleStageEffect::InitDescriptions, ;);

	/**
	 * Apply the effect.
	 * 
	 * @param Params	Information that determines how the effect should be applied. @see FApplyEffectParams 
	 */
	virtual void ApplyEffect(FApplyEffectParams& Params) const PURE_VIRTUAL(USingleStageEffect::ApplyEffect, ;);

	/** Get all IDs saved by the effect or subeffects. */
	UE_NODISCARD virtual TSet<FEffectId> GetSaveIds() const PURE_VIRTUAL(USingleStageEffect::GetSaveId, return TSet<FEffectId>(););

	/** Get all IDs used by the effect or subeffects. */
	UE_NODISCARD virtual TSet<FUseIdInfo> GetUseIds() const PURE_VIRTUAL(USingleStageEffect::GetUseIds, return TSet<FUseIdInfo>(););

	/** Checks whether the effect saves values with the specified IDs before using them. */
	UE_NODISCARD virtual bool SavesBeforeUsingIds(const TSet<FEffectId>& Ids) const PURE_VIRTUAL(USingleStageEffect::SavesBeforeUsingIds, return false;);

	/**
	 * Swaps save IDs and use IDs for the IDs in the map.
	 * @param IdMap	Maps exisitng IDs to new IDs. 
	 */
	virtual void SwapIds(const TMap<FEffectId, FEffectId>& IdMap) PURE_VIRTUAL(USingleStageEffect::SwapIds, ;);

	/**
	 * Get all simple effects with the specified type.
	 * 
	 * @param EffectType	Type of simple effects to get. 
	 * @param OutEffects	All subeffects of the specified type.
	 */
	virtual void GetEffectsByType(const EEffectType EffectType, TArray<USimpleEffect*>& OutEffects) PURE_VIRTUAL(USingleStageEffect::GetEffectsByType, ;);

	/**
	 * Gets values used to update tooltips based on value modifiers.
	 * 
	 * @param EffectParams			Information used to apply modifiers.			 
	 * @param StatusEffectParams	Maps status effect names to their formatted descriptions. 
	 * @param EffectValueParams		Maps simple effect types to the total value of all effects of that type.
	 */
	virtual void GetTooltipParams(const FApplyEffectParams& EffectParams, TMap<FName, FText>& StatusEffectParams, TMap<EEffectType, double>& EffectValueParams) const PURE_VIRTUAL(USingleStageEffect::GetTooltipParams, ;);

	/**
	 * Remove all effects that use the specified ID.
	 * 
	 * @param UseId			Simple effects that use this ID should be removed. 
	 * @param RemoveSelf	Function that removes the current effect from its parent. 
	 */
	virtual void RemoveEffectsByUseId(const FEffectId UseId, TFunctionRef<void()> RemoveSelf) PURE_VIRTUAL(USingleStageEffect::RemoveEffectsByUseId, ;);

};
