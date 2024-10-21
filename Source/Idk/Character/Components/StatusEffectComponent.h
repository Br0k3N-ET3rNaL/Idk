// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <Containers/Map.h>
#include <Containers/Queue.h>
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <TimerManager.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "StatusEffectComponent.generated.h"

class UStatusEffectBarWidget;
class UStatusEffectWidget;
class UStatusEffectInternal;
class UAttributeSystemComponent;

DECLARE_DELEGATE_OneParam(FUpdateStacksDelegate, const uint8);
DECLARE_DELEGATE_OneParam(FUpdateTimeRemainingDelegate, const double);

/** An instance of a status effect. */
UCLASS()
class UStatusEffectInstance final : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the status effect instance.
	 * 
	 * @param InStatusEffect	Status effect managed by this instance.
	 * @param InAttributeSystem Attribute system of the character the status effect is applied to.
	 * @param InTimerManager	Timer manager used to set timers.
	 * @return					A reference to itself.
	 */
	UStatusEffectInstance& Init(const UStatusEffectInternal& InStatusEffect, UAttributeSystemComponent& InAttributeSystem, FTimerManager& InTimerManager);

	/** Called when status effect is first applied. */
	void Apply();

	/** Called when status effect is applied again. */
	void Reapply();

	/** Called by a timer to update the duration of the status effect. */
	void Update();

	/** Removes effects from attribute system. */
	void Remove();

	/** Stop timers. */
	void Stop();

	/** Checks if the status effect lasts forever. */
	bool IsPermanent() const;

	/** Get the number of stacks currently applied. */
	UE_NODISCARD uint8 GetStacks() const;

	/**
	 * Delegate called when the number of stacks changes.
	 * 
	 * @param Stacks Number of current stacks.
	 */
	FUpdateStacksDelegate UpdateStacksDelegate;

	/**
	 * Delegate called when the time remaining changes.
	 * 
	 * @param DurationRemainingPercent Percentage of time remaining for the status effect.
	 */
	FUpdateTimeRemainingDelegate UpdateTimeRemainingDelegate;

	/** Delegate that removes itself from the status effect component. */
	FSimpleDelegate RemoveSelfDelegate;

private:
	/** Status effect managed by this instance. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UStatusEffectInternal> StatusEffect;

	/** Attribute system of the character the status effect is applied to. */
	UPROPERTY()
	TObjectPtr<UAttributeSystemComponent> AttributeSystem;

	/** Time until status effect is removed if not refreshed. */
	UPROPERTY(VisibleInstanceOnly)
	double DurationRemaining = 0.0;

	/** Current number of stacks. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 Stacks = 1;

	/** Delegate called by the timer that calls Update(). */
	FTimerDelegate UpdateDelegate;

	/** Timer manager used to set timers. */
	FTimerManager* TimerManager = nullptr;

	/** Timer that updates the status effect at regular intervals. */
	FTimerHandle Timer;

	/** Timers for stacks after the first when a status effect can stack and isn't refreshable. */
	TQueue<FTimerHandle> StackTimers;

};

/** Component that manages status effects applied to a character. */
UCLASS( ClassGroup=(Custom), NotBlueprintable, meta=(BlueprintSpawnableComponent) )
class UStatusEffectComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatusEffectComponent();

	//~ Begin UActorComponent Interface.
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	//~ End UActorComponent Interface

	/** Set the attribute system of the owning character. */
	void SetAttributeSystem(UAttributeSystemComponent& InAttributeSystem);

	/** Set the widget used to display applied status effects. */
	void SetStatusEffectBarWidget(UStatusEffectBarWidget& InStatusEffectBarWidget);

	/** Apply status effect. */
	void ApplyStatusEffect(const UStatusEffectInternal& StatusEffect);

	/** Check if a status effect with the specified name is applied. */
	bool HasStatusEffect(const FName& StatusName) const;

	/** Get the number of stacks of the status effect with the specified name. */
	uint8 GetStatusEffectStacks(const FName& StatusName) const;

	/** Remove a status effect by name. */
	uint8 RemoveStatusEffect(const FName& StatusName);

	/** Remove all status effects. */
	uint8 ClearStatusEffects();

	/** Interval at which status effects are updated. */
	static constexpr double TimerInterval = 0.25;

private:
	/**
	 * Remove a status effect by name.
	 * Called via delegates from status effect instances to remove themselves.
	 * 
	 * @param StatusName			Name of the status effect to remove.
	 * @param StatusEffectWidget	Widget used by the specified status effect.
	 */
	void RemoveStatusEffectInternal(const FName StatusName, UStatusEffectWidget* StatusEffectWidget);

	/** Maps status effect names to instances. */
	UPROPERTY(VisibleInstanceOnly)
	TMap<FName, UStatusEffectInstance*> StatusEffects;

	/** Attribute system of the owning character. */
	TObjectPtr<UAttributeSystemComponent> AttributeSystem;

	/** Widget that contains individual status effect widgets. */
	UPROPERTY()
	TObjectPtr<UStatusEffectBarWidget> StatusEffectBarWidget;

};
