// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RemoveItemDelegate.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "ShopRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AShopPedestalSelector;
class FDataValidationContext;
class UItem;

/** Room the allows the player to buy items for gold. */
UCLASS(Blueprintable)
class AShopRoomManager final : public AHallRoomManager
{
	GENERATED_BODY()
	
public:
	AShopRoomManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

private:
	//~ Begin ARoomManager Interface.
	virtual void OnRoomActivated() override;
	virtual void OnCompletion() override;
	//~ End ARoomManager Interface

	/**
	 * Event called when a item is selected.
	 * 
	 * @param Index	Index of the item selected.
	 */
	void OnPedestalSelected(const uint8 Index);

	/** Blueprint class that offers items that the player can buy. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<AShopPedestalSelector> PedestalSelectorClass;

	/** Offers items that the player can buy. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AShopPedestalSelector> PedestalSelector;

	/** Generated items. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<UItem*> GeneratedItems;

	/** Prices for each of the generated items. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<int32> ItemPrices;

	/** Delegates that can be called to remove generated items from the item pool. */
	TArray<FRemoveItemDelegate> RemoveItemDelegates;

	/** Text shown to the player if they don't have enough gold to buy a selected item. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	FText NotEnoughGoldText;
};
