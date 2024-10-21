// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Items/ItemPool.h"

#include "Idk/Character/PlayerClasses.h"
#include "Idk/GameSystems/Items/FullItemPool.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include "Idk/IdkGameInstance.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Math/Color.h>
#include <Misc/AssertionMacros.h>
#include <Misc/EnumRange.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>

FWeightedItem::FWeightedItem(UItem* Item)
	: Item(Item)
{
}

UItem& FWeightedItem::GetItem() const
{
	return *Item;
}

uint8 FWeightedItem::GetWeight() const
{
	return BaseWeight * WeightMultiplier;
}

void FWeightedItem::AdjustWeight(const double WeightMultiplierBonus)
{
	WeightMultiplier += WeightMultiplierBonus;
}

void FWeightedItem::ResetWeightMultiplier()
{
	WeightMultiplier = BaseWeightMultiplier;
}

void FWeightedItem::ClearItem()
{
	Item = nullptr;
}

void FWeightedItemArray::Init(const EItemRarity Rarity, const FLinearColor& RarityColor)
{
	for (int32 i = 0; i < WeightedItems.Num(); ++i)
	{
		UItem& Item = WeightedItems[i].GetItem();

		Item.InitRarity(Rarity, RarityColor);
		Item.SetIndex(i);

		Max += WeightedItems[i].GetWeight();

		ValidIndices.Add(i);

		FGameplayTagContainer ItemTags;

		Item.GetOwnedGameplayTags(ItemTags);

		for (const FGameplayTag& ItemTag : ItemTags)
		{
			TSet<int32>& Indices = ItemTagMap.FindOrAdd(ItemTag);

			Indices.Add(i);
		}
	}

	Max -= 1;
}

void FWeightedItemArray::Append(const FDefaultItemArray& Other, UObject* Outer)
{
	for (const UItem* Item : Other.Items)
	{
		check(Item);

		WeightedItems.Emplace(UItem::DuplicateItem(Outer, *Item));
	}
}

int32 FWeightedItemArray::GetMax() const
{
	if (bGeneratingItems && !GeneratedIndices.IsEmpty())
	{
		int32 TempMax = Max;

		for (const int32 Index : GeneratedIndices)
		{
			TempMax -= WeightedItems[Index].GetWeight();
		}

		return TempMax;
	}

	return Max;
}

int32 FWeightedItemArray::Num() const
{
	return ValidIndices.Num() - GeneratedIndices.Num();
}

const UItem& FWeightedItemArray::GetItem(const int32 GeneratedNum, int32& OutIndex) const
{
	check(GeneratedNum <= Max);
	check(bGeneratingItems);

	int32 Num = GeneratedNum;

	for (int32 i : ValidIndices)
	{
		if (GeneratedIndices.IsEmpty() || !GeneratedIndices.Contains(i))
		{
			const int32 Weight = WeightedItems[i].GetWeight();

			if (Num < Weight)
			{
				OutIndex = i;
				GeneratedIndices.Add(i);

				return WeightedItems[i].GetItem();
			}

			Num -= Weight;
		}
	}

	OutIndex = WeightedItems.Num() - 1;
	GeneratedIndices.Add(OutIndex);

	return WeightedItems.Top().GetItem();
}

FName FWeightedItemArray::RemoveItem(const int32 Index)
{
	ValidIndices.Remove(Index);

	FGameplayTagContainer ItemTags;

	WeightedItems[Index].GetItem().GetOwnedGameplayTags(ItemTags);

	for (const FGameplayTag& ItemTag : ItemTags)
	{
		if (TSet<int32>* TaggedItemIndices = ItemTagMap.Find(ItemTag))
		{
			TaggedItemIndices->Remove(Index);
		}
	}

	Max -= WeightedItems[Index].GetWeight();

	FName ItemName = WeightedItems[Index].GetItem().GetItemName();

	WeightedItems[Index].ClearItem();

	return ItemName;
}

const UItem* FWeightedItemArray::GetItemFromIndex(const int32 Index)
{
	if (WeightedItems.IsValidIndex(Index))
	{
		return &WeightedItems[Index].GetItem();
	}
	
	return nullptr;
}

const UItem* FWeightedItemArray::GetItemFromName(const FName& Name)
{
	for (const FWeightedItem& WeightedItem : WeightedItems)
	{
		const UItem& Item = WeightedItem.GetItem();

		if (Item.GetItemName().IsEqual(Name) && !Item.GetItemName().IsNone())
		{
			return &Item;
		}
	}

	return nullptr;
}

bool FWeightedItemArray::RemoveSavedItem(const int32 Index, const FName& Name)
{
	if (WeightedItems.IsValidIndex(Index))
	{
		const UItem& Item = WeightedItems[Index].GetItem();

		if (Item.GetItemName().IsEqual(Name) && !Item.GetItemName().IsNone())
		{
			RemoveItem(Index);
			return true;
		}
	}

	return false;
}

bool FWeightedItemArray::RemoveSavedItem(const FName& Name)
{
	for (int32 i = 0; i < WeightedItems.Num(); ++i)
	{
		const UItem& Item = WeightedItems[i].GetItem();

		if (Item.GetItemName().IsEqual(Name) && !Item.GetItemName().IsNone())
		{
			RemoveItem(i);

			return true;
		}
	}

	return false;
}

void FWeightedItemArray::StartGeneratingItems() const
{
	check(!bGeneratingItems);

	bGeneratingItems = true;
}

void FWeightedItemArray::StopGeneratingItems() const
{
	check(bGeneratingItems);

	bGeneratingItems = false;

	GeneratedIndices.Empty();
}

void FWeightedItemArray::Empty()
{
	Max = 0;

	WeightedItems.Empty();
	ValidIndices.Empty();
}

void FWeightedItemArray::AdjustWeights(const FGameplayTag& ItemTag, const double WeightMultiplierBonus)
{
	if (const TSet<int32>* TaggedItemIndices = ItemTagMap.Find(ItemTag))
	{
		for (const int32 Index : *TaggedItemIndices)
		{
			WeightedItems[Index].AdjustWeight(WeightMultiplierBonus);
		}
	}
}

void FWeightedItemArray::ResetWeightMultipliers()
{
	for (const int32 Index : ValidIndices)
	{
		WeightedItems[Index].ResetWeightMultiplier();
	}
}

void FBackupItemArray::Init(const EItemRarity Rarity, const FLinearColor& RarityColor, const int32 BaseIndex)
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		UItem& Item = *Items[i];

		Item.InitRarity(Rarity, RarityColor);
		Item.SetIndex(BaseIndex + i);
	}
}

void FBackupItemArray::Append(const FDefaultItemArray& Other, UObject* Outer)
{
	for (const UItem* Item : Other.Items)
	{
		check(Item);

		UItem* NewItem = UItem::DuplicateItem(Outer, *Item);

		NewItem->EnableStacking();
		Items.Add(NewItem);
	}
}

int32 FBackupItemArray::Num() const
{
	return Items.Num() - GeneratedIndices.Num();
}

const UItem& FBackupItemArray::GetItem(const int32 GeneratedNum) const
{
	check(bGeneratingItems);
	check(GeneratedNum < Num());

	if (GeneratedIndices.IsEmpty())
	{
		GeneratedIndices.Add(GeneratedNum);

		return *Items[GeneratedNum];
	}

	int32 Index = GeneratedNum;

	for (int32 i = GeneratedNum; i < Items.Num(); ++i)
	{
		if (!GeneratedIndices.Contains(i))
		{
			GeneratedIndices.Add(i);

			return *Items[i];
		}
	}

	ensureAlwaysMsgf(false, TEXT("All existing items already generated"));

	return *Items[0];
}

const UItem* FBackupItemArray::GetItemFromIndex(const int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		return Items[Index];
	}

	return nullptr;
}

const UItem* FBackupItemArray::GetItemFromName(const FName& Name)
{
	for (const UItem* Item : Items)
	{
		if (Item->GetItemName().IsEqual(Name) && !Item->GetItemName().IsNone())
		{
			return Item;
		}
	}

	return nullptr;
}

void FBackupItemArray::StartGeneratingItems() const
{
	check(!bGeneratingItems);

	bGeneratingItems = true;
}

void FBackupItemArray::StopGeneratingItems() const
{
	check(bGeneratingItems);

	bGeneratingItems = false;

	GeneratedIndices.Empty();
}

void FBackupItemArray::Empty()
{
	Items.Empty();
}

void UItemPool::Init(const UFullItemPool* FullItemPool, const EPlayerClass PlayerClass)
{
	check(PlayerClass != EPlayerClass::None && PlayerClass != EPlayerClass::Num);
	check(FullItemPool);

	Items.Empty();
	BackupItems.Empty();

	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items.FindOrAdd(Rarity).Append(FullItemPool->SharedItems.Items[Rarity], this);
		Items.FindOrAdd(Rarity).Append(FullItemPool->ClassItems[PlayerClass].Items[Rarity], this);
		Items[Rarity].Init(Rarity, FullItemPool->RarityColors[Rarity]);

		BackupItems.Add(Rarity).Append(FullItemPool->BackupItems[Rarity], this);
		BackupItems[Rarity].Init(Rarity, FullItemPool->RarityColors[Rarity], Items[Rarity].Num());
	}
}

int32 UItemPool::GetMax(const EItemRarity Rarity) const
{
	return Items[Rarity].GetMax();
}

int32 UItemPool::Num(const EItemRarity Rarity) const
{
	return Items[Rarity].Num();
}

int32 UItemPool::NumBackup(const EItemRarity Rarity) const
{
	return BackupItems[Rarity].Num();
}

UItem& UItemPool::GetItem(const EItemRarity Rarity, const int32 GeneratedNum, int32& OutIndex)
{
	const UItem& Item = Items[Rarity].GetItem(GeneratedNum, OutIndex);

	return *UItem::DuplicateItem(this, Item);
}

UItem& UItemPool::GetBackupItem(const EItemRarity Rarity, const int32 GeneratedNum)
{
	const UItem& Item = BackupItems[Rarity].GetItem(GeneratedNum);

	return *UItem::DuplicateItem(this, Item);
}

void UItemPool::RemoveItem(const EItemRarity Rarity, const int32 Index)
{
	const FName& ItemName = Items[Rarity].RemoveItem(Index);

	UIdkGameInstance::GetGameInstance(GetWorld())->SaveRemovedItem(FItemSaveData(ItemName, Rarity, Index, true));
}

UItem& UItemPool::RestoreSavedItem(const FItemSaveData& ItemSaveData)
{
	const UItem& Item = GetItemFromSaveData(ItemSaveData);

	UItem* NewItem = UItem::DuplicateItem(this, Item);

	if (ItemSaveData.IsDisabled())
	{
		NewItem->DisableItem();
	}

	return *NewItem;
}

void UItemPool::RemoveSavedItems(const TArray<FItemSaveData>& RemovedItemSaveData)
{
	for (const FItemSaveData& ItemSaveData : RemovedItemSaveData)
	{
		if (!Items[ItemSaveData.GetRarity()].RemoveSavedItem(ItemSaveData.GetIndex(), ItemSaveData.GetName()))
		{
			for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
			{
				if (Items[Rarity].RemoveSavedItem(ItemSaveData.GetName()))
				{
					break;
				}
			}
		}
	}
}

void UItemPool::StartGeneratingItems() const
{
	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items[Rarity].StartGeneratingItems();
		BackupItems[Rarity].StartGeneratingItems();
	}
}

void UItemPool::StopGeneratingItems() const
{
	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items[Rarity].StopGeneratingItems();
		BackupItems[Rarity].StopGeneratingItems();
	}
}

void UItemPool::AdjustWeights(const FGameplayTag& ItemTag, const double WeightMultiplierBonus)
{
	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items[Rarity].AdjustWeights(ItemTag, WeightMultiplierBonus);
	}
}

void UItemPool::ResetWeightMultipliers()
{
	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items[Rarity].ResetWeightMultipliers();
	}
}

void UItemPool::Clear()
{
	Items.Empty();
	BackupItems.Empty();
}

const UItem& UItemPool::GetItemFromSaveData(const FItemSaveData& ItemSaveData)
{
	const UItem* SavedItem;

	const int32 NumItemsOfRarity = Items[ItemSaveData.GetRarity()].Num();

	if (ItemSaveData.GetIndex() >= NumItemsOfRarity)
	{
		SavedItem = BackupItems[ItemSaveData.GetRarity()].GetItemFromIndex(ItemSaveData.GetIndex() - NumItemsOfRarity);
	}
	else
	{
		SavedItem = Items[ItemSaveData.GetRarity()].GetItemFromIndex(ItemSaveData.GetIndex());
	}

	if (SavedItem && SavedItem->MatchesSaveData(ItemSaveData))
	{
		return *SavedItem;
	}
	else
	{
		for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
		{
			SavedItem = Items[Rarity].GetItemFromName(ItemSaveData.GetName());

			if (SavedItem && SavedItem->MatchesSaveData(ItemSaveData))
			{
				return *SavedItem;
			}

			SavedItem = BackupItems[Rarity].GetItemFromName(ItemSaveData.GetName());

			if (SavedItem && SavedItem->MatchesSaveData(ItemSaveData))
			{
				return *SavedItem;
			}
		}
	}

	checkf(false, TEXT("Saved item \"%s\" no longer exists"), *ItemSaveData.GetName().ToString());

	return *SavedItem;
}
