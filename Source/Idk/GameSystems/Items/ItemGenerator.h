// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RemoveItemDelegate.h"
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "ItemGenerator.generated.h"

enum class EDataValidationResult : uint8;
enum class EItemRarity : uint8;
enum class EPlayerClass : uint8;
class FDataValidationContext;
class UCurveTable;
class UFullItemPool;
class UItem;
class UItemPool;
struct FGameplayTag;
struct FItemSaveData;

/** Workaround to allow a TMap to be used in a delegate. */
#define COMMA ,
DECLARE_DELEGATE_TwoParams(FGetItemTagCountsDelegate, TMap<FGameplayTag COMMA int32>&, int32&);
#undef COMMA

/** Used to randomly generate items for the player. */
UCLASS(Blueprintable)
class UItemGenerator final : public UObject
{
	GENERATED_BODY()

public:
	UItemGenerator();
	
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the item generator based on the specified player class.
	 * 
	 * @param PlayerClass	Class of the current player.
	 */
	void Init(const EPlayerClass PlayerClass);

	/**
	 * Randomly generate items.
	 * 
	 * @param Seed					Seed to use for the random number generator.
	 * @param NumItems				Number of items to generate.
	 * @param Level					Determines chances for higher rarity items.
	 * @param OutItems				Generated items.
	 * @param RemoveItemDelegates	Delegates that can be used to remove the corresponding item from the item pool.
	 */
	void GenerateItems(const int32 Seed, const uint8 NumItems, const uint8 Level, TArray<UItem*>& OutItems, TArray<FRemoveItemDelegate>& RemoveItemDelegates);

	/**
	 * Randomly generate cursed items.
	 * 
	 * @param Seed					Seed to use for the random number generator.
	 * @param NumItems				Number of items to generate.
	 * @param OutItems				Generated items.
	 * @param RemoveItemDelegates	Delegates that can be used to remove the corresponding item from the item pool.
	 */
	void GenerateCursedItems(const int32 Seed, const uint8 NumItems, TArray<UItem*>& OutItems, TArray<FRemoveItemDelegate>& RemoveItemDelegates);

	/** Remove items from the item pool based on save data. */
	void RemoveSavedItems(const TArray<FItemSaveData>& RemovedItemSaveData);

	/**
	 * Get items based on save data.
	 * @param SavedItemData	Save data of the items to get.
	 * @param OutItems		The items.
	 */
	void GetSavedItems(const TArray<FItemSaveData>& SavedItemData, TArray<UItem*>& OutItems);

	/** Reset the item pool. */
	void Reset();

	/**
	 * Gets the item tags of items in the player's inventory, along with the 
	 * number of items with each tag.
	 * 
	 * @param ItemTagCounts TMap where the key is an item tag, and the value is number of items with that tag.
	 * @param ItemCount		Total number of items.
	 */
	FGetItemTagCountsDelegate GetItemTagCountsDelegate;

private:
	/**
	 * Remove an item from the item pool.
	 * 
	 * @param Rarity	Rarity of the item to remove.
	 * @param Index		Index of the item to remove.
	 */
	void RemoveItem(const EItemRarity Rarity, const int32 Index);

	/**
	 * Randomly generate an item.
	 * 
	 * @param Seed					Seed to use for the random number generator.
	 * @param Level					Determines chances for higher rarity items.
	 * @param RemoveItemDelegate	Delegate that can be used to remove the item from the item pool.
	 * @return						Generated item.
	 */
	UE_NODISCARD UItem& GenerateItem(const int32 Seed, const uint8 Level, FRemoveItemDelegate& RemoveItemDelegate);

	/**
	 * Randomly generate an item with the specified rarity.
	 * 
	 * @param Seed					Seed to use for the random number generator.
	 * @param Rarity				Rarity of the item to generate.
	 * @param RemoveItemDelegate	Delegate that can be used to remove the item from the item pool.
	 * @return						Generated item.
	 */
	UE_NODISCARD UItem& GenerateItem(const int32 Seed, const EItemRarity Rarity, FRemoveItemDelegate& RemoveItemDelegate);

	/**
	 * Randomly generate a backup item with the specified item.
	 * 
	 * @param Seed		Seed to use for the random number generator.
	 * @param Rarity	Rarity of the item to generate.
	 * @return			Generated item.
	 */
	UE_NODISCARD UItem& GenerateBackupItem(const int32 Seed, const EItemRarity Rarity);

	/** Contains all possible items. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftObjectPtr<UFullItemPool> FullItemPool;

	/** Contains all items for the current player class. */
	UPROPERTY(VisibleInstanceOnly, Instanced)
	TObjectPtr<UItemPool> ItemPool;

	/** Curve table containing curves for the chance of generating an item with the specified rarity for each rarity. */
	UPROPERTY(EditDefaultsOnly, Category = "Item Generation|Drop Chances")
	TSoftObjectPtr<UCurveTable> RarityChanceCurveTable;

	/** Chance to upgrade the rarity if no non-backup items remain for generated rarity. */
	UPROPERTY(EditDefaultsOnly, Category = "Item Generation|Drop Chances", meta = (ClampMin = "0", ClampMax = "100", Units = "%"))
	int32 UpgradeRarityIfEmptyChance = 33;

	/** Name of the common chance curve within RarityChanceCurveTable. */
	static const FName CommonCurveName;

	/** Name of the uncommon chance curve within RarityChanceCurveTable. */
	static const FName UncommonCurveName;

	/** Name of the rare chance curve within RarityChanceCurveTable. */
	static const FName RareCurveName;

	/** Name of the legendary chance curve within RarityChanceCurveTable. */
	static const FName LegendaryCurveName;
};
