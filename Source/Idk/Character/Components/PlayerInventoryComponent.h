// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/Components/InventoryComponent.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "PlayerInventoryComponent.generated.h"

class FName;
class UInventoryWidget;
class UItem;
struct FGameplayTag;

/** Inventory component for player characters. */
UCLASS(NotBlueprintable)
class UPlayerInventoryComponent final : public UInventoryComponent
{
	GENERATED_BODY()
	
public:
	//~ Begin UInventoryComponent Interface.
	virtual void AddItem(UItem& Item) override final;
	//~ End UInventoryComponent Interface

	/** Adds saved items to the player's inventory. */
	void RestoreInventory(const TArray<UItem*>& InItems);

	/** Set the widget used to display items in the player's inventory. */
	void SetInventoryWidget(UInventoryWidget* InInventoryWidget);

	/** Disable an item in the inventory by name. */
	void DisableItemByName(const FName& ItemName);

	/**
	 * Gets the number of items corresponding to item tags in the player's inventory.
	 * 
	 * Used by the item generator to weight items based on item's the player has.
	 * 
	 * @param OutItemTagCounts	TMap where the key is an item tag, and the value is number of items with that tag.
	 * @param OutItemCount		Total number of items.
	 */
	void GetItemTagCounts(TMap<FGameplayTag, int32>& OutItemTagCounts, int32& OutItemCount) const;

private:
	/** Widget used to display items in the player's inventory. */
	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

};
