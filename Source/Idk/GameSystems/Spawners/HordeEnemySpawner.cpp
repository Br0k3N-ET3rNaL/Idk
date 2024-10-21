// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/HordeEnemySpawner.h"

#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>

void AHordeEnemySpawner::SetHordeSpawnerInfo(const uint8 InMaxWaves)
{
	MaxWaves = InMaxWaves;
}

void AHordeEnemySpawner::SpawnWave()
{
	check(CurrentWave < MaxWaves);

	SpawnRandomEnemies(SpawnBudgetCurve->GetFloatValue(Level + CurrentWave));
}

bool AHordeEnemySpawner::AllWavesCompleted() const
{
	return CurrentWave == MaxWaves;
}

uint8 AHordeEnemySpawner::GetMaxWaves() const
{
	return MaxWaves;
}

void AHordeEnemySpawner::OnAllEnemiesKilled()
{
	++CurrentWave;

	if (CurrentWave == MaxWaves)
	{
		OnAllEnemiesKilledDelegate.ExecuteIfBound();
	}
	else
	{
		OnWaveCompleteDelgate.ExecuteIfBound();
	}
}
