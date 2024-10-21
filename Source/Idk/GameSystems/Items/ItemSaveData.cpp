// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/Items/ItemSaveData.h"

#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

FItemSaveData::FItemSaveData(const FName& Name, const EItemRarity Rarity, const int32 Index, const bool bDisabled)
	: Name(Name), Rarity(Rarity), Index(Index), bDisabled(bDisabled)
{
}

void FItemSaveData::Disable()
{
	bDisabled = true;
}

const FName& FItemSaveData::GetName() const
{
	return Name;
}

EItemRarity FItemSaveData::GetRarity() const
{
	return Rarity;
}

int32 FItemSaveData::GetIndex() const
{
	return Index;
}

bool FItemSaveData::IsDisabled() const
{
	return bDisabled;
}
