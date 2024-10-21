// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/RandomEnemySpawner.h"
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "HordeEnemySpawner.generated.h"

/** Spawner that can spawn multiple waves of enemies. */
UCLASS()
class AHordeEnemySpawner final : public ARandomEnemySpawner
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the horde spawner.
	 * 
	 * @param InMaxWaves	Maximum number of waves that can be spawned. 
	 */
	void SetHordeSpawnerInfo(const uint8 InMaxWaves);

	/** Spawn the next wave of enemies. */
	void SpawnWave();

	/** Check if all waves have been completed. */
	UE_NODISCARD bool AllWavesCompleted() const;

	/** Get the maximum number of waves. */
	UE_NODISCARD uint8 GetMaxWaves() const;

	/** Delegate called when a wave is complete. */
	FSimpleDelegate OnWaveCompleteDelgate;

private:
	//~ Begin AEnemySpawner Interface.
	virtual void OnAllEnemiesKilled() override;
	//~ End AEnemySpawner Interface

	/** Maximum number of waves that can be spawned. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 MaxWaves = 2;

	/** Current wave of enemies. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 CurrentWave = 0;
};
