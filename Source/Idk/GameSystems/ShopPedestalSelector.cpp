// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/ShopPedestalSelector.h"

#include "Idk/GameSystems/Pedestal.h"
#include "Idk/GameSystems/PedestalSelector.h"
#include <Containers/Array.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>

void AShopPedestalSelector::OnPedestalSelected(const uint8 Index)
{
	LastSelectedPedestal = Index;

	Super::OnPedestalSelected(Index);
}

void AShopPedestalSelector::ConfirmPedestalSelection(const uint8 Index)
{
	check(Index == LastSelectedPedestal);

	Pedestals[Index]->Destroy();
	Pedestals[Index] = nullptr;
}
