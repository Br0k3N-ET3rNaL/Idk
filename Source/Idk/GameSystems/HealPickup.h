// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/EffectPickup.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "HealPickup.generated.h"

class AIdkPlayerCharacter;
class USimpleEffect;

/** Pickup that heals the player when picked up. */
UCLASS()
class AHealPickup final : public AEffectPickup
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the pickup.
	 * 
	 * @param HealAmount	Amount to heal the player for when picked up. 
	 */
	void Init(const double HealAmount);
	
private:
	//~ Begin AEffectPickup Interface.
	virtual void OnPickup(AIdkPlayerCharacter& Player) override;
	//~ End AEffectPickup Interface

	/** Effect applied to the player when pickup up. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USimpleEffect> HealEffect;
};
