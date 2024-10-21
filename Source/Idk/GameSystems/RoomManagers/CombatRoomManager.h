// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "CombatRoomManager.generated.h"

class ABasicEnemySpawner;
class UCurveFloat;
class USpawnableEnemyClasses;

/** Room where the player will fight against one wave of basic enemies. */
UCLASS(Blueprintable)
class ACombatRoomManager final : public AArenaRoomManager
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * Set information used by the spawner.
	 * 
	 * @param InEnemyClasses		Data asset containing all spawnable enemy types.
	 * @param InSpawnBudgetCurve	Curve that determines the budget that the spawner has to spawn enemies.
	 */
	void SetSpawnerInfo(const USpawnableEnemyClasses& InEnemyClasses, const UCurveFloat& InSpawnBudgetCurve);

private:
	//~ Begin ARoomManager Interface.
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	//~End ARoomManager Interface

	/** Data asset containing all spawnable enemy types. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const USpawnableEnemyClasses> EnemyClasses;

	/** Curve that determines the budget that the spawner has to spawn enemies. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UCurveFloat> SpawnBudgetCurve;

	/** Used to spawn enemies for the player to fight. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ABasicEnemySpawner> EnemySpawner;
};
