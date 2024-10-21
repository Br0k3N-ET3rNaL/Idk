// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <NiagaraComponent.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "DamageNumberComponent.generated.h"

/** Component that displays floating numbers for damage or healing. */
UCLASS(NotBlueprintable)
class UDamageNumberComponent final : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	/** Display a floating number for damage. */
	void AddDamageNumber(const double DamageDone);

	/** Display a floating number for healing */
	void AddHealNumber(const double AmountHealed);

private:
	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	/**
	 * Display a floating number.
	 * 
	 * @param Value Number to display.
	 * @param bHeal Whether the value represents healing or damage.
	 */
	void AddNumber(const double Value, const bool bHeal);

	/** Name of the niagara array parameter that contains the values for floating numbers to display. */
	static const FName DamageInfoArrayName;

	/** Name of the niagara array parameter that contains whether the corresponding value is for healing or damage. */
	static const FName IsHealArrayName;

};
