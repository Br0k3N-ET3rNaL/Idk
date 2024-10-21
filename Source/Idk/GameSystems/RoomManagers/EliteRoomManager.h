// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "EliteRoomManager.generated.h"

class AEliteEnemySpawner;
class UEliteEncounters;

/** Room where the player will fight against an elite encounter. */
UCLASS(Blueprintable)
class AEliteRoomManager final : public AArenaRoomManager
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/** Set the data asset containing possible encounters that the player can fight. */
	void SetEncounters(const UEliteEncounters& Encounters);

private:
	//~ Begin ARoomManager Interface.
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	//~ End ARoomManager Interface

	/** Data asset containing possible encounters that the player can fight. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UEliteEncounters> EliteEncounters;

	/** Used to spawn the elite encounter. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AEliteEnemySpawner> Spawner;
};
