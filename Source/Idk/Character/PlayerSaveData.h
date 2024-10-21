// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Items/ItemSaveData.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "PlayerSaveData.generated.h"

class UItem;

/** Save data associated with the player. */
USTRUCT()
struct FPlayerSaveData final
{
	GENERATED_BODY()
	
public:
	/** Update save data with an item was added to the player's inventory. */
	void AddItem(const UItem& Item);

	/** Update save data with the player's current health. */
	void SetHealth(const double CurrentHealth);

	/** Update save data with the player's current level and experience. */
	void SetLevelAndExperience(const uint8 InLevel, const int32 InExperience);

	/** Update save data with the player's current gold. */
	void SetGold(const int32 InGold);

	/** Update save data with the name of the ability in the player's first ability slot. */
	void SetAbility1Name(const FName& AbilityName);

	/** Update save data with the name of the ability in the player's second ability slot. */
	void SetAbility2Name(const FName& AbiltityName);

	/** Update save data for an item that was disabled. */
	void DisableItem(const FName& ItemName);

	/** Get the save data for the player's inventory. */
	UE_NODISCARD const TArray<FItemSaveData>& GetSavedItems() const;

	/** Get the player's saved health. */
	UE_NODISCARD double GetHealth();

	/** Get the player's saved level. */
	UE_NODISCARD uint8 GetLevel() const;

	/** Get the player's saved experience. */
	UE_NODISCARD int32 GetExperience() const;

	/** Get the player's saved gold. */
	UE_NODISCARD int32 GetGold() const;

	/** Get the saved name of the ability in the player's first ability slot. */
	UE_NODISCARD const FName& GetAbility1Name() const;

	/** Get the saved name of the ability in the player's second ability slot. */
	UE_NODISCARD const FName& GetAbility2Name() const;

private:
	/** Save data for item's in the player's inventory. */
	UPROPERTY(VisibleAnywhere)
	TArray<FItemSaveData> Items;

	/** Saved player health. */
	UPROPERTY(VisibleAnywhere)
	double Health = 0.0;

	/** Saved player level. */
	UPROPERTY(VisibleAnywhere)
	uint8 Level = 1;

	/** Saved player experience. */
	UPROPERTY(VisibleAnywhere)
	int32 Experience = 0;

	/** Saved player gold. */
	UPROPERTY(VisibleAnywhere)
	int32 Gold = 0;

	/** Saved name of the ability in the player's first ability slot. */
	UPROPERTY(VisibleAnywhere)
	FName Ability1Name = NAME_None;

	/** Saved name of the ability in the player's second ability slot. */
	UPROPERTY(VisibleAnywhere)
	FName Ability2Name = NAME_None;

};
