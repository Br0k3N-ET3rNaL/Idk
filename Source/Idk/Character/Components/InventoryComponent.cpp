// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/InventoryComponent.h"

#include "Idk/GameSystems/Items/Item.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Misc/CoreMiscDefines.h>

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UItem& Item)
{
	int32 Index;

	if (Item.IsStackable())
	{
		Index = Items.IndexOfByKey(Item);

		if (Index == INDEX_NONE)
		{
			Index = Items.Add(&Item);
		}
	}
	else
	{
		Index = Items.Add(&Item);
	}

	Items[Index]->ApplyEffect(AttributeSystem);
}

void UInventoryComponent::SetAttributeSystem(UAttributeSystemComponent* InAttributeSystem)
{
	AttributeSystem = InAttributeSystem;
}
