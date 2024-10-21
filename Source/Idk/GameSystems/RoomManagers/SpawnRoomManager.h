// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include <UObject/ObjectMacros.h>

#include "SpawnRoomManager.generated.h"

/** Room that the player spawns in. */
UCLASS(Blueprintable)
class ASpawnRoomManager final : public AArenaRoomManager
{
	GENERATED_BODY()
	
private:
	//~ Begin ARoomManager Interface.
	virtual void OnEnabled() override;
	//~ End ARoomManager Interface

};
