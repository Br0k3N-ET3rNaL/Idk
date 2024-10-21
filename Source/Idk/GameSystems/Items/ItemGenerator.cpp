// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Items/ItemGenerator.h"

#include "Idk/Character/PlayerClasses.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemPool.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include "Idk/GameSystems/RemoveItemDelegate.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Curves/RealCurve.h>
#include <Engine/CurveTable.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/NameTypes.h>

const FName UItemGenerator::CommonCurveName = TEXT("Common");
const FName UItemGenerator::UncommonCurveName = TEXT("Uncommon");
const FName UItemGenerator::RareCurveName = TEXT("Rare");
const FName UItemGenerator::LegendaryCurveName = TEXT("Legendary");

UItemGenerator::UItemGenerator()
{
	ItemPool = CreateDefaultSubobject<UItemPool>(TEXT("ItemPool"));
}

#if WITH_EDITOR
EDataValidationResult UItemGenerator::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (FullItemPool.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Full item pool is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (RarityChanceCurveTable.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Rarity chance curve table is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UItemGenerator::Init(const EPlayerClass PlayerClass)
{
	check(!FullItemPool.IsNull());

	// Initialize the item pool
	ItemPool->Init(FullItemPool.LoadSynchronous(), PlayerClass);
}

void UItemGenerator::GenerateItems(const int32 Seed, const uint8 NumItems, const uint8 Level, TArray<UItem*>& OutItems, TArray<FRemoveItemDelegate>& RemoveItemDelegates)
{
	UIdkRandomStream& Rng = UIdkRandomStream::CreateRandomStream(this, Seed);

	ItemPool->ResetWeightMultipliers();

	TMap<FGameplayTag, int32> ItemTagCounts;
	int32 ItemCount = 0;

	GetItemTagCountsDelegate.Execute(ItemTagCounts, ItemCount);

	// Adjust the weights of items for each item tag in the player's inventory
	for (const TPair<FGameplayTag, int32>& TagCountPair : ItemTagCounts)
	{
		ItemPool->AdjustWeights(TagCountPair.Key, double(TagCountPair.Value) / double(ItemCount));
	}

	ItemPool->StartGeneratingItems();

	// Generate the items
	for (uint8 i = 0; i < NumItems; ++i)
	{
		RemoveItemDelegates.AddDefaulted();

		OutItems.Add(&GenerateItem(Rng.GenerateSeed(), Level, RemoveItemDelegates.Top()));
	}

	ItemPool->StopGeneratingItems();
}

void UItemGenerator::GenerateCursedItems(const int32 Seed, const uint8 NumItems, TArray<UItem*>& OutItems, TArray<FRemoveItemDelegate>& RemoveItemDelegates)
{
	UIdkRandomStream& Rng = UIdkRandomStream::CreateRandomStream(this, Seed);

	ItemPool->StartGeneratingItems();

	for (uint8 i = 0; i < NumItems; ++i)
	{
		RemoveItemDelegates.AddDefaulted();

		// Check if there are cursed items remaining
		if (ItemPool->Num(EItemRarity::Cursed) > 0)
		{
			OutItems.Add(&GenerateItem(Rng.GenerateSeed(), EItemRarity::Cursed, RemoveItemDelegates.Top()));
		}
		else
		{
			OutItems.Add(&GenerateBackupItem(Rng.GenerateSeed(), EItemRarity::Cursed));
		}
	}

	ItemPool->StopGeneratingItems();
}

void UItemGenerator::RemoveSavedItems(const TArray<FItemSaveData>& RemovedItemSaveData)
{
	ItemPool->RemoveSavedItems(RemovedItemSaveData);
}

void UItemGenerator::GetSavedItems(const TArray<FItemSaveData>& SavedItemData, TArray<UItem*>& OutItems)
{
	for (const FItemSaveData& ItemSaveData : SavedItemData)
	{
		OutItems.Add(&ItemPool->RestoreSavedItem(ItemSaveData));
	}
}

void UItemGenerator::Reset()
{
	ItemPool->Clear();
}

void UItemGenerator::RemoveItem(const EItemRarity Rarity, const int32 Index)
{
	ItemPool->RemoveItem(Rarity, Index);
}

UItem& UItemGenerator::GenerateItem(const int32 Seed, const uint8 Level, FRemoveItemDelegate& RemoveItemDelegate) 
{
	UIdkRandomStream& Rng = UIdkRandomStream::CreateRandomStream(this, Seed);

	// Number used to determine the rarity of the generated item
	const uint8 Num = Rng.RandRange(0, 99);
	EItemRarity Rarity = EItemRarity::None;

	check(!RarityChanceCurveTable.IsNull());

	const UCurveTable* CurveTable = RarityChanceCurveTable.LoadSynchronous();

	// Get the rarity chance curves
	const FRealCurve* CommonDropChanceCurve = CurveTable->FindCurve(CommonCurveName, {});
	const FRealCurve* UncommonDropChanceCurve = CurveTable->FindCurve(UncommonCurveName, {});
	const FRealCurve* RareDropChanceCurve = CurveTable->FindCurve(RareCurveName, {});
	//const FRealCurve* LegendaryDropChanceCurve = CurveTable->FindCurveUnchecked(LegendaryCurveName);

	// Get the rarity chance for the specified level
	const float CommonDropChance = CommonDropChanceCurve->Eval(Level);
	const float UncommonDropChance = UncommonDropChanceCurve->Eval(Level);
	const float RareDropChance = RareDropChanceCurve->Eval(Level);
	//const float LegendaryDropChance = LegendaryDropChanceCurve->Eval(Level);

	// Get the cutoffs an item of the specified rarity to be generated
	const uint8 CommonDropChanceCutoff = FMath::Floor(CommonDropChance);
	const uint8 UncommonDropChanceCutoff = FMath::Floor(CommonDropChance + UncommonDropChance);
	const uint8 RareDropChanceCutoff = FMath::Floor(CommonDropChance + UncommonDropChance + RareDropChance);
	//const uint8 LegendaryDropChanceCutoff = FMath::Floor()

	// Get the number of items remaining for each rarity
	const int32 CommonItemsRemaining = ItemPool->Num(EItemRarity::Common);
	const int32 UncommonItemsRemaining = ItemPool->Num(EItemRarity::Uncommon);
	const int32 RareItemsRemaining = ItemPool->Num(EItemRarity::Rare);
	const int32 LegendaryItemsRemaining = ItemPool->Num(EItemRarity::Legendary);

	EItemRarity GeneratedRarity = EItemRarity::None;

	// Determine what rarity the item should be
	if (Num < CommonDropChanceCutoff)
	{
		GeneratedRarity = EItemRarity::Common;
	}
	else if (Num < UncommonDropChanceCutoff)
	{
		GeneratedRarity = EItemRarity::Uncommon;
	}
	else if (Num < RareDropChanceCutoff)
	{
		GeneratedRarity = EItemRarity::Rare;
	}
	else
	{
		GeneratedRarity = EItemRarity::Legendary;
	}

	const int32 NextSeed = Rng.GenerateSeed();

	// Whether to upgrade the rarity if there are no non-backup items remaining of the generated rarity
	const bool bUpgradeRarityIfEmpty = Rng.RandRange(1, 100) <= UpgradeRarityIfEmptyChance;

	Rarity = GeneratedRarity;

	if (bUpgradeRarityIfEmpty)
	{
		// Upgrade the rarity of the item if no items remain at the current rarity
		if (Rarity == EItemRarity::Common && CommonItemsRemaining == 0)
		{
			Rarity = EItemRarity::Uncommon;
		}
		if (Rarity == EItemRarity::Uncommon && UncommonItemsRemaining == 0)
		{
			Rarity = EItemRarity::Rare;
		}
		if (Rarity == EItemRarity::Rare && RareItemsRemaining == 0)
		{
			Rarity = EItemRarity::Legendary;
		}
		if (Rarity == EItemRarity::Legendary && LegendaryItemsRemaining == 0)
		{
			Rarity = GeneratedRarity;

			return GenerateBackupItem(NextSeed, Rarity);
		}
	}
	else if ((Rarity == EItemRarity::Common && CommonItemsRemaining == 0)
		|| (Rarity == EItemRarity::Uncommon && UncommonItemsRemaining == 0)
		|| (Rarity == EItemRarity::Rare && RareItemsRemaining == 0)
		|| (Rarity == EItemRarity::Legendary && LegendaryItemsRemaining == 0))
	{
		return GenerateBackupItem(NextSeed, Rarity);
	}

	return GenerateItem(NextSeed, Rarity, RemoveItemDelegate);
}

UItem& UItemGenerator::GenerateItem(const int32 Seed, const EItemRarity Rarity, FRemoveItemDelegate& RemoveItemDelegate)
{
	const int32 Num = UIdkRandomStream::RandRange(Seed, 0, ItemPool->GetMax(Rarity));

	int32 GeneratedIndex = -1;
	UItem& GeneratedItem = ItemPool->GetItem(Rarity, Num, GeneratedIndex);

	RemoveItemDelegate.BindUObject(this, &UItemGenerator::RemoveItem, Rarity, GeneratedIndex);

	return GeneratedItem;
}

UItem& UItemGenerator::GenerateBackupItem(const int32 Seed, const EItemRarity Rarity)
{
	const int32 Num = UIdkRandomStream::RandRange(Seed, 0, ItemPool->NumBackup(Rarity) - 1);

	return ItemPool->GetBackupItem(Rarity, Num);
}
