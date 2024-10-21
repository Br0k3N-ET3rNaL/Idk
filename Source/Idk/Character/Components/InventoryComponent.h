// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <Containers/Array.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "InventoryComponent.generated.h"

class UAttributeSystemComponent;
class UItem;

/** Component that manages items for a character. */
UCLASS( Abstract, ClassGroup=(Custom) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	/** Add an item to the inventory. */
	virtual void AddItem(UItem& Item);

	/** Set the attribute system of the owner. */
	void SetAttributeSystem(UAttributeSystemComponent* InAttributeSystem);

protected:
	/** Items in inventory. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<UItem*> Items;

	/** Attribute system of the owner. */
	TObjectPtr<UAttributeSystemComponent> AttributeSystem;

};
