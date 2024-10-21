// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/PedestalSelector.h"
#include <HAL/Platform.h>
#include <Math/NumericLimits.h>
#include <UObject/ObjectMacros.h>

#include "ShopPedestalSelector.generated.h"

/** Pedestal selector that allows the player to select multiple options, but each item has a price. */
UCLASS()
class AShopPedestalSelector final : public APedestalSelector
{
	GENERATED_BODY()
	
public:
	//~ Begin APedestalSelector Interface.
	virtual void OnPedestalSelected(const uint8 Index) override;
	//~ End APedestalSelector Interface

	/**
	 * Called after checking that the player can afford the selected item.
	 * 
	 * @param Index	Index of the selected pedestal. 
	 */
	void ConfirmPedestalSelection(const uint8 Index);

private:
	/** Index of the last selected pedestal. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 LastSelectedPedestal = MAX_uint8;
};
