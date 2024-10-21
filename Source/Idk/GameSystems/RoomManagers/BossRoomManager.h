// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "BossRoomManager.generated.h"

class ABossEnemySpawner;
class UBossEncounters;

/** Final room where the player will fight a boss. */
UCLASS()
class ABossRoomManager : public AArenaRoomManager
{
	GENERATED_BODY()
	
public:
	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/** Set the possible boss encounters that can be spawned. */
	void SetEncounters(const UBossEncounters& Encounters);

private:
	//~ Begin ARoomManager Interface.
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	//~ End ARoomManager Interface

	//~ Begin AArenaRoomManager Interface.
	virtual void OnAllEnemiesKilled() override;
	//~ End AArenaRoomManager Interface

	/** Possible boss encounters that can be spawned. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UBossEncounters> BossEncounters;

	/** Spawner used to spawn the boss encounter. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ABossEnemySpawner> Spawner;
	
};
