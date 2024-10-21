// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/EnemySpawner.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "EliteEnemySpawner.generated.h"

class UEliteEncounters;

/** Spawner that spawns elite encounters. */
UCLASS()
class AEliteEnemySpawner final : public AEnemySpawner
{
	GENERATED_BODY()

public:
	/** Set the data asset containing possible encounters that the player can fight. */
	void SetEncounters(const UEliteEncounters& InEliteEncounters);

	/** Spawn a random elite encounter. */
	void SpawnEliteEncounter();

private:
	/** Data asset containing possible encounters that the player can fight. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UEliteEncounters> EliteEncounters;
	
};
