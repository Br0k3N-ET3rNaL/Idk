// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "InventoryWidget.generated.h"

class UIdkItem;
class UItem;
class UItemWidget;
class UWrapBox;

/** Widget that displays the player's current inventory. */
UCLASS(Abstract)
class UInventoryWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Add an item to display.
	 * 
	 * @param Item	Item to add. 
	 * @param Index Index to insert the item at.
	 */
	void InsertItem(const UItem& Item, const int32 Index);

	/**
	 * Set the number of stacks for an item.
	 * 
	 * @param Index		Index of the item to set the stacks for. 
	 * @param Stacks	New number of stacks for the item.
	 */
	void SetItemStacks(const int32 Index, const uint8 Stacks);

	/** Disable the item at the specified index. */
	void DisableItem(const int32 Index);

	/** Add multiple items to display. */
	void AddItems(const TArray<UItem*>& Items);

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Wrap box containing widgets for each item in the player's inventory. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox;

	/** Class for widgets used to display an item. @see UItemWidget */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UItemWidget> ItemWidgetClass;
	
};
