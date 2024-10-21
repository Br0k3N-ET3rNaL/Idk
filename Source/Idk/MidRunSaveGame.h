// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/PlayerSaveData.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <Delegates/Delegate.h>
#include <GameFramework/SaveGame.h>
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

#include "MidRunSaveGame.generated.h"

enum class EPathTaken : uint8;
enum class EPlayerClass : uint8;
class AIdkPlayerCharacter;

/** Saves information related to the current run. */
UCLASS()
class UMidRunSaveGame final : public USaveGame
{
	GENERATED_BODY()

public:
	/** Set the seed for the current run. */
	void SetSeed(const int32 InSeed);

	/** Set the class of the current player character. */
	void SetPlayerClass(const TSubclassOf<AIdkPlayerCharacter> InPlayerClass);

	/** Set the class of the current player character. */
	void SetPlayerClassEnum(const EPlayerClass InPlayerClassEnum);

	/**
	 * Saves the player's progress through the map.
	 *
	 * @param PathTaken	Represents the path the player took.
	 */
	void UpdatePlayerProgress(const EPathTaken PathTaken);

	/** Save an item that was removed from the item pool. */
	void SaveRemovedItem(const FItemSaveData& ItemSaveData);

	/** Set save data related to the current player. */
	void SetPlayerSaveData(const FPlayerSaveData& InPlayerSaveData);

	/** Get the saved seed. */
	UE_NODISCARD int32 GetSeed() const;

	/** Get the player's saved progress. */
	UE_NODISCARD const TArray<EPathTaken>& GetPlayerProgress() const;

	/** Get the class of the saved player character. */
	UE_NODISCARD TSubclassOf<AIdkPlayerCharacter> GetPlayerClass() const;

	/** Get the class of the saved player character. */
	UE_NODISCARD EPlayerClass GetPlayerClassEnum() const;

	/** Get item save data for each item removed from the item pool. */
	UE_NODISCARD const TArray<FItemSaveData>& GetRemovedItemSaveData() const;

	/** Get save data related to the current player. */
	UE_NODISCARD const FPlayerSaveData& GetPlayerSaveData() const;

	/** Mark the save as completed. */
	void SetSaveAsComplete();

	/** Check if the save is valid. */
	UE_NODISCARD bool IsValid() const;

	/** Check if the save is complete. */
	UE_NODISCARD bool IsComplete() const;

	/** Check if saving is in progress. */
	UE_NODISCARD bool IsSaving() const;

	/** Save current save data. */
	void SaveAsync();

	/** Create a new midrun save. */
	UE_NODISCARD static UMidRunSaveGame& CreateMidRunSave();

	/** Load existing midrun save if it exists and is valid. */
	UE_NODISCARD static UMidRunSaveGame* LoadMidRunSave();

	/** Delete the current midrun save. */
	static void DeleteMidRunSave();

	/** Delegate called when saving has completed. */
	FSimpleDelegate OnSaveCompleteDelegate;

	/** Name of the save game slot to save to. */
	static const FString SlotName;

	/** Index of the user to use when saving. */
	static const int32 UserIndex;

	/** Current midrun save version. */
	static const int32 CurrentSaveVersion = 0;
	
private:
	/** Event called when saving has completed. */
	void OnSaveComplete(const FString& InSlotName, const int32 InUserIndex, bool bSuccessful) const;

	/** Saved seed. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	int32 Seed = 0;

	/** Saved player progress. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	TArray<EPathTaken> PlayerProgress;

	/** Saved player class. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	TSubclassOf<AIdkPlayerCharacter> PlayerClass;

	/** Saved player class. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	EPlayerClass PlayerClassEnum = EPlayerClass();

	/** Item save data for items removed from the item pool. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	TArray<FItemSaveData> RemovedItemSaveData;

	/** Save data related to the current player. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	FPlayerSaveData PlayerSaveData;

	/** Whether the save is complete. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	bool bComplete = false;

	/** Save version used by the save. */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	int32 SaveVersion = CurrentSaveVersion;

	/** Whether saving is currently in progress. */
	mutable bool bSaving = false;

};
