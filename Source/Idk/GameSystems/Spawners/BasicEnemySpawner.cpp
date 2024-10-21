// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/BasicEnemySpawner.h"

void ABasicEnemySpawner::SpawnEnemies()
{
	SpawnRandomEnemies(SpawnBudgetCurve->GetFloatValue(Level));
}
