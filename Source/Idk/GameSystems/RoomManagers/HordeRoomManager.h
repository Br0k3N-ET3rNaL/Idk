// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "HordeRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AConfirmationBox;
class AHordeEnemySpawner;
class FDataValidationContext;
class UCurveFloat;
class USpawnableEnemyClasses;
struct FRoomSpawnInfo;

/** Room where the player can fight against multiple waves of enemies. @see AHordeEnemySpawner */
UCLASS(Blueprintable)
class AHordeRoomManager final : public AArenaRoomManager
{
	GENERATED_BODY()

public:
	AHordeRoomManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

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
	//~ End ARoomManager Interface

	//~ Begin AArenaRoomManager Interface.
	virtual void OnAllEnemiesKilled() override;
	//~ End AArenaRoomManager Interface

	/** Event that is called when a wave completes. */
	void OnWaveComplete();

	/** Blueprint class that allows the player choose to spawn another wave or not. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<AConfirmationBox> ConfirmationBoxClass;

	/** Maximum number of waves to allow the player to fight. */
	UPROPERTY(EditDefaultsOnly)
	uint8 MaxWaves = 2;

	// TODO: Change to FText
	/** Text displayed to the player asking if they want to spawn another wave or not. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	FText NextWaveConfirmationText;

	/** Added to the room level to determine the level of rewards based on waves completed. */
	UPROPERTY(EditDefaultsOnly)
	uint8 PerWaveRewardBonus = 1;

	/** Data asset containing all spawnable enemy types. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const USpawnableEnemyClasses> EnemyClasses;

	/** Curve that determines the budget that the spawner has to spawn enemies. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<const UCurveFloat> SpawnBudgetCurve;

	/** Added to the room level to determine the level of rewards if all waves are completed. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	uint8 CompletionRewardBonus = MaxWaves;

	/** Used to spawn enemies for the player to fight. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AHordeEnemySpawner> EnemySpawner;

	/** Allows the player choose to spawn another wave or not. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AConfirmationBox> ConfirmationBox;
};
