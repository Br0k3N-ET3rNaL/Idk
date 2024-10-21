// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Items/ItemBuilder.h"

#include "Idk/GameSystems/Items/Item.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <GameplayTagContainer.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UnrealTemplate.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UObjectGlobals.h>

FItemBuilder FItemBuilder::Begin(UObject* Outer, FName&& Name)
{
	return FItemBuilder(NewObject<UItem>(Outer, Name, Outer->GetMaskedFlags(RF_PropagateToSubObjects)));
}

FItemBuilder FItemBuilder::BeginDefault(UObject* Outer, FName&& Name)
{
	return FItemBuilder(Outer->CreateDefaultSubobject<UItem>(Name));
}

FItemBuilder&& FItemBuilder::Init(FGenericDisplayInfo&& DisplayInfo)&&
{
	Item->DisplayInfo = MoveTemp(DisplayInfo);

	return MoveTemp(*this);
}

FItemBuilder&& FItemBuilder::AddTag(const FGameplayTag& Tag)&&
{
	Item->Tags.AddTag(Tag);

	return MoveTemp(*this);
}

UItem* FItemBuilder::Complete()&&
{
	Item->RemoveFromRoot();

	return Item;
}

FItemBuilder::FItemBuilder(UItem* Item)
	: Item(Item)
{
	check(Item);

	Item->AddToRoot();

	Item->Name = Item->GetFName();
}
