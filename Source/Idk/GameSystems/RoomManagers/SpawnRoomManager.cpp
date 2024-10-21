// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/SpawnRoomManager.h"

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"

void ASpawnRoomManager::OnEnabled()
{
	Super::OnEnabled();

	Activate();

	Complete();
}
