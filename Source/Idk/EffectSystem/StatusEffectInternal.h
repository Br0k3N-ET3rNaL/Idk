// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "StatusEffectInternal.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UAttributeAlteringEffect;
class UAttributeSystemComponent;
class UCompositeItemEffect;
class UObject;
class USimpleEffect;
class USingleStageEffectGroup;
struct FApplyEffectParams;
struct FPropertyChangedEvent;
struct FUseIdInfo;

/** Effect that can apply persistent effects and repeatedly apply instantaneous effects for a duration. */
UCLASS()
class UStatusEffectInternal : public UEffect
{
	GENERATED_BODY()

	friend class FUniqueStatusEffectBuilder;
	friend class UStatusEffectInstance;

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override final;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override final;
	//~ End UEffect Interface

	/** Get the text used to represent the status effect in a tooltip. */
	UE_NODISCARD virtual FText GetTooltipText() const;

	/** Initialize subeffect descriptions based on modifiers from the specified attribute system. */
	void InitDescriptions(UAttributeSystemComponent& AttributeSystem);

	/**
	 * Apply the effect.
	 *
	 * @param Params	Information that determines how the effect should be applied. @see FApplyEffectParams
	 */
	void ApplyEffect(FApplyEffectParams& Params) const;

	/**
	 * Apply any instantaneous effects to the target.
	 * 
	 * @param Target	Target to apply effects to.
	 * @param Stacks	Current number of stacks.
	 * @param Interval	Amount of time since the last time the status effect was activated.
	 */
	void ActivateEffect(UAttributeSystemComponent* Target, const uint8 Stacks, const double Interval) const;

	/** Apply any persistent effects to the target. */
	void ApplyPersistentEffects(UAttributeSystemComponent* Target) const;

	/**
	 * Remove any persistent effects from the target.
	 *
	 * @param Target	Target to remove effects from.
	 * @param Stacks	Number of stacks to remove.
	 */
	void RemoveAttributeAlteringEffects(UAttributeSystemComponent* Target, const uint8 Stacks) const;

	/** Get the status effect's name. */
	UE_NODISCARD const FName& GetName() const;

	/** Get the information used to display the status effect. */
	UE_NODISCARD const FGenericDisplayInfo& GetDisplayInfo() const;

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

protected:
	/** Name of the status effect. Should be unique. */
	UPROPERTY(EditAnywhere)
	FName Name = FName();

	/** Information used to display the status effect to the player. */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FGenericDisplayInfo DisplayInfo;

	/** Duration of the status effect. -1.0 represents infinite duration. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0"))
	double Duration = 1.0;

	/** Maximum number of stacks that can be applied to a single target. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	uint8 MaxStacks = 1;

	/** Whether the duration of the status effect is refreshed when reapplied. */
	UPROPERTY(EditAnywhere)
	bool bRefreshable = false;

	/** Instantaneous effects that are applied at a regular interval. */
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<USingleStageEffectGroup> RepeatedEffects;

	/** Persistent effects that are applied/removed when the status effect is. */
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UCompositeItemEffect> PersistentEffects;

	/** Used to represent a status effect with infinite duration. */
	static constexpr double PermanentDuration = -1.0;

private:
	/**
	 * Gets values used to update tooltips based on value modifiers.
	 *
	 * @param InOutArgs	Maps parameter names to values.
	 * @param Self		Attribute system used to apply modifiers.	
	 */
	void GetTooltipParams(FFormatNamedArguments& InOutArgs, UAttributeSystemComponent* Self) const;

	/** Check whether the status effect has any persistent effects. */
	UE_NODISCARD bool HasPersistentEffects() const;

	/** Text used to embed the status effect's name and description within a tooltip. @note Wraps name and description with a rich text decorator. */
	static const FTextFormat TooltipFormatText;

};
