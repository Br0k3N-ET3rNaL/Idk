// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/UI/InventoryWidget.h"

#include "Idk/GameSystems/Items/Item.h"
#include "Idk/UI/ItemWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/Widget.h>
#include <Components/WrapBox.h>
#include <Components/WrapBoxSlot.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>

void UInventoryWidget::InsertItem(const UItem& Item, const int32 Index)
{
	check(!ItemWidgetClass.IsNull());

	UItemWidget* ItemWidget = CreateWidget<UItemWidget>(this, ItemWidgetClass.LoadSynchronous());
	ItemWidget->Init(Item.GetDisplayInfo());

	// Check if the item should be added to the end
	if (WrapBox->GetChildrenCount() == Index)
	{
		WrapBox->AddChildToWrapBox(ItemWidget);
	}
	else
	{
		// Save existing items
		TArray<UWidget*> Widgets = WrapBox->GetAllChildren();

		// Insert the new item
		Widgets.Insert(WrapBox->AddChildToWrapBox(ItemWidget)->Content, Index);

		// Clear existing items
		WrapBox->ClearChildren();

		// Re-add widgets in sorted order
		for (UWidget* Widget : Widgets)
		{
			WrapBox->AddChildToWrapBox(Widget);
		}
	}

	if (Item.IsDisabled())
	{
		DisableItem(Index);
	}

	WrapBox->InvalidateLayoutAndVolatility();
}

void UInventoryWidget::SetItemStacks(const int32 Index, const uint8 Stacks)
{
	CastChecked<UItemWidget>(WrapBox->GetChildAt(Index))->SetItemStacks(Stacks);

	WrapBox->InvalidateLayoutAndVolatility();
}

void UInventoryWidget::DisableItem(const int32 Index)
{
	CastChecked<UItemWidget>(WrapBox->GetChildAt(Index))->DisableItem();

	WrapBox->InvalidateLayoutAndVolatility();
}

void UInventoryWidget::AddItems(const TArray<UItem*>& Items)
{
	for (const UItem* Item : Items)
	{
		InsertItem(*Item, WrapBox->GetChildrenCount());
	}
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Remove preview items
	WrapBox->ClearChildren();
	WrapBox->InvalidateLayoutAndVolatility();
}
