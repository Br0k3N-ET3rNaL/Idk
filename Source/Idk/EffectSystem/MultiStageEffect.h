// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectIdSet.h"
#include "Idk/EffectSystem/MultiStageEffectContainer.h"
#include <Containers/Array.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Templates/PimplPtr.h>
#include <UObject/ObjectMacros.h>

#include "MultiStageEffect.generated.h"

enum class EDataValidationResult : uint8;
enum class EEffectStage : uint8;
enum class EEffectType : uint8;
class FDataValidationContext;
class FName;
class UAttributeSystemComponent;
class UMultiStageBonusEffect;
class UObject;
struct FApplyEffectParams;
struct FEffectSavedValues;
struct FPartialEffectInfo;

/** Effect that contains other effects that can be applied to self and targets. @see EEffectStage */
UCLASS()
class UMultiStageEffect final : public UEffect
{
	GENERATED_BODY()

	friend class FMultiStageEffectBuilder;

	friend class UIdkGameInstance;

public:
	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	/** Initialize effect descriptions based on modifiers from the specified attribute system. */
	void InitDescriptions(UAttributeSystemComponent& AttributeSystem);

	/**
	 * Apply self effects for conditional effects (OnDamage, OnHeal, etc.). 
	 * 
	 * @param Self						Attribute system to apply effects to. 
	 * @param ConditionalEffectValue	Value of the effect which triggered this effect.
	 */
	void ApplyEffectsToSelf(UAttributeSystemComponent& Self, const double ConditionalEffectValue) const;

	/**
	 * Apply self effects.
	 * 
	 * @param Self					Attribute system to apply effects to.  
	 * @param EffectOrigin			Where the effect is being applied from.
	 */
	void ApplyEffectsToSelf(UAttributeSystemComponent& Self, const FVector2D EffectOrigin) const;

	/**
	 * Apply target effects to multiple targets.
	 * 
	 * @param Self				Attribute system of the character applying the effect. 
	 * @param Targets			Attribute systems of targets.
	 * @param EffectOrigin		Where the effect is being applied from.
	 * @param PerHitMultiplier	Multiplier applied to effects which are applied multiple times. @note Used by status effects and abilities with multiple hits.
	 */
	void ApplyEffectsToTargets(UAttributeSystemComponent& Self, TArray<UAttributeSystemComponent*>& Targets, const FVector2D EffectOrigin, const double PerHitMultiplier = 1.0) const;

	/**
	 * Apply target effects to a single target.
	 * 
	 * @param Self				Attribute system of the character applying the effect.
	 * @param Targets			Target's attribute system.
	 * @param EffectOrigin		Where the effect is being applied from.
	 * @param PerHitMultiplier	Multiplier applied to effects which are applied multiple times. @note Used by status effects and abilities with multiple hits.
	 */
	void ApplyEffectsToTarget(UAttributeSystemComponent& Self, UAttributeSystemComponent& Target, const FVector2D EffectOrigin, const double PerHitMultiplier = 1.0) const;

	/**
	 * Apply target effects to a single target for conditional effects (OnDamage, OnHeal, etc.)
	 * @param Self						Attribute system of the character applying the effect.
	 * @param Targets					Target's attribute system.
	 * @param ConditionalEffectValue	Value of the effect which triggered this effect.
	 */
	void ApplyEffectsToTarget(UAttributeSystemComponent& Self, UAttributeSystemComponent& Target, const double ConditionalEffectValue) const;

	/**
	 * Add a multistage bonus effect.
	 * 
	 * @param InBonusEffect	Bonus effect to add.
	 * @param InOutEffectId ID of the multistage bonus effect.
	 * @param bNewId		Whether InOutEffectId should be treated as a new ID even if it's already set.
	 */
	void AddBonusEffects(const UMultiStageBonusEffect& InBonusEffect, FEffectId& InOutEffectId, const bool bNewId = false);

	/** Remove all bonus effects with the specified ID. */
	void RemoveBonusEffects(const FEffectId BonusEffectId);

	/** Remove all bonus effects. */
	void RemoveAllBonusEffects();

	/**
	 * Add a new partial effect.
	 * 
	 * @param PartialEffectInfo	Information representing the partial effect. @see FPartialEffectInfo 
	 * @param OutBonusEffectId	ID used by the bonus effect. 
	 * @param OutSaveId			ID used by subeffects. 
	 */
	void AddPartialEffect(const FPartialEffectInfo& PartialEffectInfo, FEffectId& OutBonusEffectId, FEffectId& OutSaveId);

	/**
	 * Add a stack of an existing partial effect.
	 * 
	 * @param PartialEffectInfo	Information representing the partial effect. @see FPartialEffectInfo  
	 * @param BonusEffectId		ID used by the bonus effect. 
	 */
	void AddPartialEffectStack(const FPartialEffectInfo& PartialEffectInfo, const FEffectId BonusEffectId);

	/**
	 * Remove a partial effect.
	 * 
	 * @param PartialEffectInfo	Information representing the partial effect. @see FPartialEffectInfo   
	 * @param BonusEffectId		ID used by the bonus effect. 
	 * @param SaveId			ID used by subeffects. 
	 */
	void RemovePartialEffect(const FPartialEffectInfo& PartialEffectInfo, const FEffectId BonusEffectId, const FEffectId SaveId);

	/** Allow target effects to be added even if there are no target effects by default. */
	void AllowAddingTargetEffects();

	/** Whether the multistage effect has any subeffects. */
	UE_NODISCARD bool HasEffects() const;

	/** Whether target effects can be added. */
	UE_NODISCARD bool CanHaveTargetEffects() const;

	/**
	 * Gets values used to update ability tooltips based on modifiers.
	 * @param Args		Maps parameter names to values. 
	 * @param Self		Attribute system used to apply modifiers.
	 * @param Duration	Duration of the ability. 
	 * @param Interval	How often the ability applies its effects.
	 */
	void GetTooltipParams(FFormatNamedArguments& Args, UAttributeSystemComponent* Self, const double Duration, const double Interval) const;

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/**
	 * Apply all effects within a stage.
	 * 
	 * @param Stage		Stage of effects to apply.
	 * @param Params	Information used to apply effects. @see FApplyEffectParams	
	 */
	void ApplyStageEffects(const EEffectStage Stage, FApplyEffectParams& Params) const;

	/** Get all IDs saved by subeffects. */
	UE_NODISCARD TSet<FEffectId> GetSaveIds() const;

	/** Contains all subeffects. */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FMultiStageEffectContainer Effects;

	/** Set of IDs used by applied bonus effects. */
	UPROPERTY(VisibleInstanceOnly, meta = (ShowOnlyInnerProperties))
	FEffectIdSet BonusEffectIds;

	/** Values saved by self effects. */
	mutable TPimplPtr<FEffectSavedValues> SelfSavedValues = nullptr;

	/** Whether target effects can be added even if there are no target effects by default. */
	UPROPERTY()
	bool bCanHaveTargetEffects = false;

};
