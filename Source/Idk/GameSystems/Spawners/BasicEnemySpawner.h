// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/RandomEnemySpawner.h"
#include <UObject/ObjectMacros.h>

#include "BasicEnemySpawner.generated.h"

/** Basic enemy spawner. */
UCLASS()
class ABasicEnemySpawner final : public ARandomEnemySpawner
{
	GENERATED_BODY()
	
public:
	/** Spawn enemies based on the current level and the spawn budget curve. */
	void SpawnEnemies();

};
