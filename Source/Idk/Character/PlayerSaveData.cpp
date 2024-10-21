// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/PlayerSaveData.h"

#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

void FPlayerSaveData::AddItem(const UItem& Item)
{
	Items.Emplace(Item.GetItemSaveData());
}

void FPlayerSaveData::SetHealth(const double CurrentHealth)
{
	Health = CurrentHealth;
}

void FPlayerSaveData::SetLevelAndExperience(const uint8 InLevel, const int32 InExperience)
{
	Level = InLevel;
	Experience = InExperience;
}

void FPlayerSaveData::SetGold(const int32 InGold)
{
	Gold = InGold;
}

void FPlayerSaveData::SetAbility1Name(const FName& AbilityName)
{
	Ability1Name = AbilityName;
}

void FPlayerSaveData::SetAbility2Name(const FName& AbilityName)
{
	Ability2Name = AbilityName;
}

void FPlayerSaveData::DisableItem(const FName& ItemName)
{
	// Find and the disable the item with the specified name
	for (FItemSaveData& ItemSaveData : Items)
	{
		if (ItemSaveData.GetName().IsEqual(ItemName))
		{
			ItemSaveData.Disable();
		}
	}
}

const TArray<FItemSaveData>& FPlayerSaveData::GetSavedItems() const
{
	return Items;
}

double FPlayerSaveData::GetHealth()
{
	return Health;
}

uint8 FPlayerSaveData::GetLevel() const
{
	return Level;
}

int32 FPlayerSaveData::GetExperience() const
{
	return Experience;
}

int32 FPlayerSaveData::GetGold() const
{
	return Gold;
}

const FName& FPlayerSaveData::GetAbility1Name() const
{
	return Ability1Name;
}

const FName& FPlayerSaveData::GetAbility2Name() const
{
	return Ability2Name;
}
