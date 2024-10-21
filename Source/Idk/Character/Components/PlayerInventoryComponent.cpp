// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/Components/PlayerInventoryComponent.h"

#include "Idk/Character/Components/InventoryComponent.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/UI/InventoryWidget.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/NameTypes.h>

void UPlayerInventoryComponent::AddItem(UItem& Item)
{
	const bool bDuplicate = Items.Contains(Item);

	Super::AddItem(Item);

	Items.Sort();

	const int32 Index = Items.IndexOfByKey(Item);

	check(Index != INDEX_NONE);
	check(Items[Index]);

	const UItem& AddedItem = *Items[Index];

	if (bDuplicate && AddedItem.IsStackable())
	{
		InventoryWidget->SetItemStacks(Index, AddedItem.GetStacks());
	}
	else
	{
		InventoryWidget->InsertItem(AddedItem, Index);
	}
}

void UPlayerInventoryComponent::RestoreInventory(const TArray<UItem*>& InItems)
{
	for (UItem* Item : InItems)
	{
		Super::AddItem(*Item);
	}

	Items.Sort();

	InventoryWidget->AddItems(Items);

	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const UItem* Item = Items[i];

		if (Item->IsStackable() && Item->GetStacks() > 1)
		{
			InventoryWidget->SetItemStacks(i, Item->GetStacks());
		}
	}
}

void UPlayerInventoryComponent::SetInventoryWidget(UInventoryWidget* InInventoryWidget)
{
	InventoryWidget = InInventoryWidget;
}

void UPlayerInventoryComponent::DisableItemByName(const FName& ItemName)
{
	const int32 ItemIndex = Items.IndexOfByPredicate([ItemName](const UItem* Item)
		{
			return Item->GetItemName().IsEqual(ItemName);
		});

	if (ItemIndex != INDEX_NONE)
	{
		Items[ItemIndex]->DisableItem();
		Items[ItemIndex]->RemoveEffect(AttributeSystem);
		InventoryWidget->DisableItem(ItemIndex);
	}
}

void UPlayerInventoryComponent::GetItemTagCounts(TMap<FGameplayTag, int32>& OutItemTagCounts, int32& OutItemCount) const
{
	OutItemCount = 0;

	for (const UItem* Item : Items)
	{
		FGameplayTagContainer  ItemTags;

		Item->GetOwnedGameplayTags(ItemTags);

		for (const FGameplayTag& ItemTag : ItemTags)
		{
			++OutItemTagCounts.FindOrAdd(ItemTag, 0);
		}

		++OutItemCount;
	}
}
