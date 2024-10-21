// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RewardPedestalSelector.h"

#include "Idk/GameSystems/Pedestal.h"
#include "Idk/GameSystems/PedestalSelector.h"
#include <Containers/Array.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>

void ARewardPedestalSelector::OnPedestalSelected(const uint8 Index)
{
	Super::OnPedestalSelected(Index);

	for (uint8 i = 0; i < NumPedestals; i++)
	{
		Pedestals[i]->Destroy();
		Pedestals[i] = nullptr;
	}
}
