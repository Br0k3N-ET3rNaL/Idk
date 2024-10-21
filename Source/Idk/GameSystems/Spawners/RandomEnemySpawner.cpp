// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/Spawners/RandomEnemySpawner.h"

#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Spawners/EnemySpawner.h"
#include "Idk/GameSystems/Spawners/SpawnableEnemyClasses.h"
#include <Containers/Array.h>
#include <Curves/CurveFloat.h>
#include <HAL/Platform.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>

void ARandomEnemySpawner::SetSpawnerInfo(const USpawnableEnemyClasses& InEnemyClasses, const UCurveFloat& InSpawnBudgetCurve)
{
	EnemyClasses = &InEnemyClasses;
	SpawnBudgetCurve = &InSpawnBudgetCurve;
}

void ARandomEnemySpawner::SpawnRandomEnemies(const int32 InSpawnBudget)
{
	check(EnemyClasses);

	const int32 MinSpawnCost = EnemyClasses->EnemyTypes[0].GetSpawnCost();

	// Check if any enemies could be spawned
	if (InSpawnBudget < MinSpawnCost)
	{
		OnAllEnemiesKilledDelegate.ExecuteIfBound();
	}

	int32 SpawnBudgetRemaining = InSpawnBudget;

	// Try to spawn a single more costly enemy before switching to spawning randomly

	// Minimum cost for an enemy to be considered large based on the spawn budget
	const int32 LargeEnemyMin = FMath::Min(SpawnBudgetRemaining / 4, MinSpawnCost);

	// Maximum cost for an enemy to be considered large based on the spawn budget
	const int32 LargeEnemyMax = FMath::Max((2 * SpawnBudgetRemaining) / 3, MinSpawnCost);

	// Get all enemies that qualify as large based on the spawn budget
	const TArray<FEnemyTypeInfo>& LargeEnemies = EnemyClasses->EnemyTypes.FilterByPredicate([=](const FEnemyTypeInfo& EnemyTypeInfo)
		{
			const int32 SpawnCost = EnemyTypeInfo.GetSpawnCost();

			return SpawnCost >= LargeEnemyMin && SpawnCost <= LargeEnemyMax;
		});

	const uint8 EnemyLevel = Level;

	// Check if any enemies qualified as large
	if (!LargeEnemies.IsEmpty())
	{
		// Spawn a random large enemy
		SpawnEnemyInternal(LargeEnemies[Rng->RandRange(0, LargeEnemies.Num() - 1)], EnemyLevel, SpawnBudgetRemaining);
	}

	// Maximum index of an enemies that could be spawned based on remaining spawn budget
	int32 MaxIndex = EnemyClasses->EnemyTypes.Num() - 1;

	// Spawn enemies until spawn budget is depleted
	while (SpawnBudgetRemaining >= MinSpawnCost)
	{
		MaxIndex = EnemyClasses->EnemyTypes.FindLastByPredicate([=](const FEnemyTypeInfo& EnemyTypeInfo)
			{
				return EnemyTypeInfo.GetSpawnCost() <= SpawnBudgetRemaining;
			}, MaxIndex + 1);

		// Spawn a random affordable enemy
		SpawnEnemyInternal(EnemyClasses->EnemyTypes[Rng->RandRange(0, MaxIndex)], EnemyLevel, SpawnBudgetRemaining);
	}
}

void ARandomEnemySpawner::SpawnEnemyInternal(const FEnemyTypeInfo& EnemyTypeInfo, const uint8 EnemyLevel, int32& InOutSpawnBudget)
{
	++LivingEnemies;

	InOutSpawnBudget -= EnemyTypeInfo.GetSpawnCost();

	SpawnEnemy(EnemyTypeInfo.GetEnemyClass(), EnemyLevel);
}
