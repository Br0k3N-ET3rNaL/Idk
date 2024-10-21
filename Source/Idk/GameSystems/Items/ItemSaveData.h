// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "ItemSaveData.generated.h"

enum class EItemRarity : uint8;

/** Data needed to recreate an item. */
USTRUCT()
struct FItemSaveData final
{
	GENERATED_BODY()

public:
	FItemSaveData() = default;

	/**
	 * @param Name		Name of the saved item.
	 * @param Rarity	Rarity of the saved item.
	 * @param Index		Index of the saved item within the item pool.
	 * @param bDisabled Whether the saved item is disabled.
	 */
	FItemSaveData(const FName& Name, const EItemRarity Rarity, const int32 Index, const bool bDisabled);

	/** Disable the save item. */
	void Disable();

	/** Get the name of the saved item. */
	UE_NODISCARD const FName& GetName() const;

	/** Get the rarity of the saved item. */
	UE_NODISCARD EItemRarity GetRarity() const;

	/** Get the index of the saved item within the item pool. */
	UE_NODISCARD int32 GetIndex() const;

	/** Check if the saved item is disabled. */
	UE_NODISCARD bool IsDisabled() const;

private:
	/** Name of the saved item. */
	UPROPERTY(EditDefaultsOnly)
	FName Name;

	/** Rarity of the saved item. */
	UPROPERTY(EditDefaultsOnly)
	EItemRarity Rarity = EItemRarity();

	/** Index of the saved item within the item pool. */
	UPROPERTY(EditDefaultsOnly)
	int32 Index = -1;

	/** Whether the saved item is disabled. */
	UPROPERTY(EditDefaultsOnly)
	bool bDisabled = false;
};

