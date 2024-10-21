// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "EnemySpawner.generated.h"

class AIdkEnemyCharacter;
class AIdkPlayerCharacter;
class UIdkRandomStream;

/** Base class for enemy spawners. */
UCLASS(Abstract, NotBlueprintable)
class AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * Initialize the spawner.
	 * 
	 * @param InPlayer	Current player. 
	 * @param InLevel	Level of the spawner. 
	 * @param InSeed	Seed to use for RNG.
	 */
	void Init(AIdkPlayerCharacter& InPlayer, const uint8 InLevel, const int32 InSeed);

	/** Delegate called when all enemies have been killed. */
	FSimpleDelegate OnAllEnemiesKilledDelegate;

protected:
	/** Event called when all enemies have been killed. */
	virtual void OnAllEnemiesKilled();

	/**
	 * Event called when an enemy dies.
	 * 
	 * @param Experience	Experience to give to the player. 
	 * @param Gold			Gold to give to the player.
	 */
	void OnEnemyDeath(const int32 Experience, const int32 Gold);

	/**
	 * Spawn an enemy after an optional delay.
	 * 
	 * @param EnemyClass	Blueprint class of the enemy to spawn.
	 * @param EnemyLevel	Level of the enemy to spawn.
	 * @param SpawnDelay	Delay before spawning enemy.
	 */
	void SpawnEnemy(TSubclassOf<AIdkEnemyCharacter> EnemyClass, const uint8 EnemyLevel, const float SpawnDelay = 0.f);

	/** Level of the spawner. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 Level = 0;

	/** Seed to use for RNG */
	UPROPERTY(VisibleInstanceOnly)
	int32 Seed = 0;

	/** Alphanumeric string representing the seed. */
	UPROPERTY(VisibleInstanceOnly)
	FString SeedString;

	/** Random number generator used by the spawner. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UIdkRandomStream> Rng;

	/** Number of living enemies spawned by the spawner. */
	uint8 LivingEnemies = 0;

	/** Current player. */
	TObjectPtr<AIdkPlayerCharacter> Player;

private:
	/**
	 * Spawn an enemy.
	 * 
	 * @param EnemyClass	Blueprint class of the enemy to spawn. 
	 * @param EnemyLevel	Level of the enemy to spawn. 
	 */
	void SpawnEnemyInternal(TSubclassOf<AIdkEnemyCharacter> EnemyClass, const uint8 EnemyLevel);

	/** Living enemies. */
	TArray<AIdkEnemyCharacter*> Enemies;

};
