// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "IdkHealthComponent.generated.h"

class UHealthBarWidget;

/** Component that manages a character's health. */
UCLASS( ClassGroup=(Custom), NotBlueprintable, meta=(BlueprintSpawnableComponent) )
class UIdkHealthComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIdkHealthComponent();

	/** Get the current amount of health. */
	UE_NODISCARD double GetHealth() const;

	/** Get the current maximum health. */
	UE_NODISCARD double GetMaxHealth() const;

	/** Get the current health percentage from 0.0 to 1.0. */
	UE_NODISCARD double GetHealthPercent() const;

	/**
	 * Set maximum health.
	 * 
	 * @param MaxHealth New maximum health value.
	 * @param bHeal		Whether to heal the difference when the new value is greater than the old.
	 */
	void SetMaxHealth(const double MaxHealth, bool bHeal = true);

	/** Set the widget used to display the health. */
	void SetHealthBarWidget(UHealthBarWidget& InHealthBarWidget);

	/** Subtract from current health. */
	double TakeDamage(const double Damage);

	/** Add to current health. Won't exceed maximum health. */
	double Heal(const double HealAmount);

	/** Restore current health from saved value. */
	void RestoreSavedHealth(const double SavedHealth);

private:
	/** Update the health bar widget with current values. */
	void UpdateWidget();

	/** Current amount of health. */
	UPROPERTY(VisibleAnywhere)
	double Health = 1.0;

	/** Current maximum amount of health. */
	UPROPERTY(VisibleAnywhere)
	double MaxHealth = 1.0;

	/** Widget used to display the health */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UHealthBarWidget> HealthBarWidget;
		
};
