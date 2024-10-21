// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "ItemPool.generated.h"

enum class EPlayerClass : uint8;
class UFullItemPool;
struct FDefaultItemArray;
struct FDefaultItemGroup;
struct FLinearColor;

/** An item with a weight. */
USTRUCT()
struct FWeightedItem final
{
	GENERATED_BODY()

public:
	FWeightedItem() = default;
	FWeightedItem(UItem* Item);

	/** Get the item. */
	UE_NODISCARD UItem& GetItem() const;

	/** Get the current weight of the item. */
	UE_NODISCARD uint8 GetWeight() const;

	/**
	 * Adjust the weight of the item.
	 * 
	 * @param WeightMultiplierBonus Bonus added to the item's weight multiplier.
	 *								@see GetWeight() for how weight is calculated.
	 */
	void AdjustWeight(const double WeightMultiplierBonus);

	/** Reset the item's weight multiplier. Effectively sets item's weight to default values. */
	void ResetWeightMultiplier();

	/** Clear the item. */
	void ClearItem();
	
	/** Base value for an item's weight. */
	static const uint8 BaseWeight = 10;

	/** Base multiplier for an item's weight */
	static constexpr double BaseWeightMultiplier = 1.0;

private:
	/** The item. */
	UPROPERTY(VisibleInstanceOnly, Instanced)
	TObjectPtr<UItem> Item;

	/** Item's weight multiplier. Item's weight is equal to BaseWeight * WeightMultiplier. */
	UPROPERTY(VisibleInstanceOnly)
	double WeightMultiplier = BaseWeightMultiplier;
};

/** An array of items with weights. */
USTRUCT()
struct FWeightedItemArray final
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the array and all items within.
	 * 
	 * @param Rarity		Rarity of items within the array.
	 * @param RarityColor	Color used to display the rarity.
	 */
	void Init(const EItemRarity Rarity, const FLinearColor& RarityColor);

	/**
	 * Append items from the full item pool.
	 * 
	 * @param Other Array of items from the full item pool.
	 * @param Outer Outer object used to duplicate added items.
	 */
	void Append(const FDefaultItemArray& Other, UObject* Outer);

	/** Get the sum of all item weights within the array. */
	UE_NODISCARD int32 GetMax() const;

	/** Get the number of items within the array. */
	UE_NODISCARD int32 Num() const;

	/**
	 * Get an item from the array based on a generated number.
	 * @note See implementation for additional details.
	 * 
	 * @param GeneratedNum	Randomly generated number in the range [0, GetMax()].
	 * @param OutIndex		Index of the generated item.
	 * @return				Generated item.
	 */
	UE_NODISCARD const UItem& GetItem(const int32 GeneratedNum, int32& OutIndex) const;

	/**
	 * Remove an item from the array.
	 * 
	 * @param Index Index of the item to remove. Must be an index returned from GetItem().
	 * @return		Name of the removed item.
	 */
	FName RemoveItem(const int32 Index);

	/** Get the item with the specified index. Will return null if index is out of bounds. */
	UE_NODISCARD const UItem* GetItemFromIndex(const int32 Index);

	/** Get the item with the specified name. Will return null if no item has the specified name. */
	UE_NODISCARD const UItem* GetItemFromName(const FName& Name);

	/**
	 * Remove an item from the array.
	 * 
	 * @param Index	Index of the item within the array.
	 * @param Name	Name of the item.
	 * @return		Whether the item was found and removed.
	 */
	bool RemoveSavedItem(const int32 Index, const FName& Name);

	/**
	 * Remove an item from the array.
	 * 
	 * @param Name	Name of the item.
	 * @return		Whether the item was found and removed.
	 */
	bool RemoveSavedItem(const FName& Name);

	/** Should be called before calling GetItem(). */
	void StartGeneratingItems() const;

	/** Should be called after finished calling GetItem(). */
	void StopGeneratingItems() const;

	/** Empty the array. */
	void Empty();
	
	/**
	 * Adjust the weights of all items with the specified tag.
	 * 
	 * @param ItemTag				Tag of items to adjust.
	 * @param WeightMultiplierBonus	Bonus for item's weight multiplier.
	 */
	void AdjustWeights(const FGameplayTag& ItemTag, const double WeightMultiplierBonus);

	/** Reset weight multipliers for all items. */
	void ResetWeightMultipliers();

private:
	/** The weighted items. */
	UPROPERTY(VisibleInstanceOnly, meta = (ShowOnlyInnerProperties))
	TArray<FWeightedItem> WeightedItems;

	/** Valid indices for weighted items. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<int32> ValidIndices;

	/** Maps item tags to indices of items with the tag. */
	TMap<FGameplayTag, TSet<int32>> ItemTagMap;

	/** Sum of all item weights within the array.  */
	UPROPERTY(VisibleInstanceOnly)
	int32 Max = 0;

	/** Indices of items that have been generated with GetItem(). Reset after calling StopGeneratingItems(). */
	mutable TSet<int32> GeneratedIndices;

	/** Whether items are currently being generated. */
	mutable bool bGeneratingItems = false;
};

/** Array of backup items. */
USTRUCT()
struct FBackupItemArray final
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the array and all items within.
	 * 
	 * @param Rarity		Rarity of the items within the array.
	 * @param RarityColor	Color used to display the rarity.
	 * @param BaseIndex		Offset for index used by item's save data.
	 */
	void Init(const EItemRarity Rarity, const FLinearColor& RarityColor, const int32 BaseIndex = 0);

	/**
	 * Append backup items from the full item pool.
	 *
	 * @param Other Array of backup items from the full item pool.
	 * @param Outer Outer object used to duplicate added items.
	 */
	void Append(const FDefaultItemArray& Other, UObject* Outer);

	/** Get the number of backup items within the array. */
	UE_NODISCARD int32 Num() const;

	/**
	 * Get a backup item from the array based on a generated number.
	 * 
	 * @param GeneratedNum	Randomly generated number in the range [0, Num()].
	 * @return				Generated item.
	 */
	UE_NODISCARD const UItem& GetItem(const int32 GeneratedNum) const;

	/** Get the item with the specified index. Will return null if index is out of bounds. */
	UE_NODISCARD const UItem* GetItemFromIndex(const int32 Index);

	/** Get the item with the specified name. Will return null if no item has the specified name. */
	UE_NODISCARD const UItem* GetItemFromName(const FName& Name);

	/** Should be called before calling GetItem(). */
	void StartGeneratingItems() const;

	/** Should be called after finished calling GetItem(). */
	void StopGeneratingItems() const;

	/** Empty the array. */
	void Empty();

protected:
	/** Backup items. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<UItem*> Items;

	/** Indices of items that have been generated with GetItem(). Reset after calling StopGeneratingItems(). */
	mutable TSet<int32> GeneratedIndices;

	/** Whether items are currently being generated. */
	mutable bool bGeneratingItems = false;
};

/** Item pool for the current player class. */
UCLASS()
class UItemPool final : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the item pool for the specified player class.
	 * 
	 * @param FullItemPool	Pool of all possible items.
	 * @param PlayerClass	Player class to use for the item pool.	 
	 */
	void Init(const UFullItemPool* FullItemPool, const EPlayerClass PlayerClass);

	/** Get the maximum number that can be generated that corresponds to a non-backup item for the specified rarity. */
	UE_NODISCARD int32 GetMax(const EItemRarity Rarity) const;

	/** Get the number of non-backup items for the specified rarity. */
	UE_NODISCARD int32 Num(const EItemRarity Rarity) const;

	/** Get the number of backup items for the specified rarity. */
	UE_NODISCARD int32 NumBackup(const EItemRarity Rarity) const;

	/**
	 * Get a non-backup item with the specified rarity based on a generated number.
	 *
	 * @param Rarity		Rarity of the generated item.
	 * @param GeneratedNum	Randomly generated number in the range [0, GetMax()].
	 * @param OutIndex		Index of the generated item.
	 * @return				The generated item.
	 */
	UE_NODISCARD UItem& GetItem(const EItemRarity Rarity, const int32 GeneratedNum, int32& OutIndex);

	/**
	 * Get a backup item with specified rarity based on a generated number.
	 *
	 * @param Rarity		Rarity of the generated item.
	 * @param GeneratedNum	Randomly generated number in the range [0, NumBackup()].
	 * @return				The generated backup item.
	 */
	UE_NODISCARD UItem& GetBackupItem(const EItemRarity Rarity, const int32 GeneratedNum);

	/**
	 * Remove and item with the specified rarity and index.
	 *
	 * @param Rarity	Rarity of the item to remove.
	 * @param Index		Index of the item to remove.
	 */
	void RemoveItem(const EItemRarity Rarity, const int32 Index);

	/** Restore an item based on the specified item save data. */
	UE_NODISCARD UItem& RestoreSavedItem(const FItemSaveData& ItemSaveData);

	/** Remove all items based on the specified item save data. */
	void RemoveSavedItems(const TArray<FItemSaveData>& RemovedItemSaveData);

	/** Should be called before calling GetItem(). */
	void StartGeneratingItems() const;

	/** Should be called after finished calling GetItem(). */
	void StopGeneratingItems() const;

	/**
	 * Adjust the weights of all items with the specified tag.
	 *
	 * @param ItemTag				Tag of items to adjust.
	 * @param WeightMultiplierBonus	Bonus for item's weight multiplier.
	 */
	void AdjustWeights(const FGameplayTag& ItemTag, const double WeightMultiplierBonus);

	/** Reset weight multipliers for all items. */
	void ResetWeightMultipliers();

	/** Remove all items from the item pool. */
	void Clear();

private:
	/** Get an item based on the specified item save data. */
	UE_NODISCARD const UItem& GetItemFromSaveData(const FItemSaveData& ItemSaveData);

	/** Non-backup items. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EItemRarity, FWeightedItemArray> Items;

	/** Backup items. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EItemRarity, FBackupItemArray> BackupItems;
};
