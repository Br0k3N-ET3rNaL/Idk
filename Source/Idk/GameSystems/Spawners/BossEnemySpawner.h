// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/EnemySpawner.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "BossEnemySpawner.generated.h"

class UBossEncounters;

/** Spawner that spawns boss encounters. */
UCLASS()
class ABossEnemySpawner final : public AEnemySpawner
{
	GENERATED_BODY()
	
public:
	/** Set the possible boss encounters that can be spawned. */
	void SetEncounters(const UBossEncounters& InBossEncounters);

	/** Spawn a random boss encounter. */
	void SpawnBoss();

private:
	/** Possible boss encounters that can be spawned. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UBossEncounters> BossEncounters;
};
