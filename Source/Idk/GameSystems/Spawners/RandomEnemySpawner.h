// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/EnemySpawner.h"
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "RandomEnemySpawner.generated.h"

class AIdkEnemyCharacter;
class AIdkPlayerCharacter;
class UCurveFloat;
class USpawnableEnemyClasses;
struct FEnemyTypeInfo;

/** Base class for spawners that spawn random enemies based on a budget. */
UCLASS(Abstract)
class ARandomEnemySpawner : public AEnemySpawner
{
	GENERATED_BODY()
	
public:	
	/**
	 * Initialize the random enemy spawner.
	 * 
	 * @param InEnemyClasses		Data asset containing all spawnable enemy types. 
	 * @param InSpawnBudgetCurve	Curve that determines the budget that the spawner has to spawn enemies.
	 */
	void SetSpawnerInfo(const USpawnableEnemyClasses& InEnemyClasses, const UCurveFloat& InSpawnBudgetCurve);

protected:
	/**
	 * Randomly spawn enemies until budget is depleted.
	 * 
	 * @param InSpawnBudget	Total value of enemies that can be spawned. 
	 */
	void SpawnRandomEnemies(const int32 InSpawnBudget);

	/** Data asset containing all spawnable enemy types.  */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const USpawnableEnemyClasses> EnemyClasses;

	/** Curve that determines the budget that the spawner has to spawn enemies. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UCurveFloat> SpawnBudgetCurve;

private:
	/**
	 * Spawn an enemy.
	 * 
	 * @param EnemyTypeInfo		Information about the enemy to spawn. 
	 * @param EnemyLevel		Level of the enemy to spawn.
	 * @param InOutSpawnBudget	Spawn budget remaining.
	 */
	void SpawnEnemyInternal(const FEnemyTypeInfo& EnemyTypeInfo, const uint8 EnemyLevel, int32& InOutSpawnBudget);
};
