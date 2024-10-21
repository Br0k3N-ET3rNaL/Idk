// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/PedestalSelector.h"
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "RewardPedestalSelector.generated.h"

/** Pedestal selector that only allows the player to select one option. */
UCLASS()
class ARewardPedestalSelector final : public APedestalSelector
{
	GENERATED_BODY()
	
public:
	//~ Begin APedestalSelector Interface.
	virtual void OnPedestalSelected(const uint8 Index) override;
	//~ End APedestalSelector Interface
};
