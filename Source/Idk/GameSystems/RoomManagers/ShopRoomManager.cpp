// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/ShopRoomManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemGenerator.h"
#include "Idk/GameSystems/PedestalSelector.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include "Idk/GameSystems/ShopPedestalSelector.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/UI/PedestalDisplayInfo.h"
#include "RoomManager.h"
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

AShopRoomManager::AShopRoomManager()
{
	NotEnoughGoldText = FText::FromString(TEXT("Not enough gold."));
}

#if WITH_EDITOR
EDataValidationResult AShopRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PedestalSelectorClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AShopRoomManager: Pedestal selector class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AShopRoomManager::Destroyed()
{
	if (PedestalSelector)
	{
		PedestalSelector->Destroy();
	}

	Super::Destroyed();
}

void AShopRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	check(!PedestalSelectorClass.IsNull());

	PedestalSelector = GetWorld()->SpawnActor<AShopPedestalSelector>(PedestalSelectorClass.LoadSynchronous());
	PedestalSelector->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	const UIdkGameInstance* GameInstance = CastChecked<UIdkGameInstance>(GetGameInstance());
	UItemGenerator& ItemGenerator = GameInstance->GetItemGenerator();

	ItemGenerator.GenerateItems(Rng->GenerateSeed(),
		PedestalSelector->GetNumPedestals(), Level, GeneratedItems, RemoveItemDelegates);

	TArray<FPedestalDisplayInfo> PedestalDisplayInfo;

	// Determine the price of each generated item
	for (const UItem* Item : GeneratedItems)
	{
		int32 BasePrice = 100;
		int32 MinVariance = 0;
		int32 MaxVariance = 0;

		// Determine the base price and variance based on rarity
		switch (Item->GetRarity())	
		{
			case EItemRarity::Common:
				BasePrice = 100;
				MinVariance = -30;
				MaxVariance = 30;
				break;
			case EItemRarity::Uncommon:
				BasePrice = 200;
				MinVariance = -60;
				MaxVariance = 60;
				break;
			case EItemRarity::Rare:
				BasePrice = 300;
				MinVariance = -90;
				MaxVariance = 90;
				break;
			case EItemRarity::Legendary:
				BasePrice = 400;
				MinVariance = -120;
				MaxVariance = 120;
				break;
			default:
				break;
		}

		const int32 Price = BasePrice + Rng->RandRange(MinVariance, MaxVariance);

		ItemPrices.Add(Price);

		PedestalDisplayInfo.Emplace(FPedestalDisplayInfo(Item->GetDisplayInfo(), Price));
	}

	PedestalSelector->Init(PedestalDisplayInfo);
	PedestalSelector->PedestalSelectedDelegate.BindUObject(this, &AShopRoomManager::OnPedestalSelected);
}

void AShopRoomManager::OnCompletion()
{
	Super::OnCompletion();

	if (PedestalSelector)
	{
		PedestalSelector->Destroy();
	}
}

void AShopRoomManager::OnPedestalSelected(const uint8 Index)
{
	check(IsValid(Player));

	const int32 ItemPrice = ItemPrices[Index];

	if (Player->CanAfford(ItemPrice))
	{
		PedestalSelector->ConfirmPedestalSelection(Index);

		RemoveItemDelegates[Index].ExecuteIfBound();

		Player->RemoveGold(ItemPrice);
		Player->AddItemToInventory(*GeneratedItems[Index]);
	}
	else
	{
		UIdkGameInstance::GetGameInstance(GetWorld())->ShowTempMessage(NotEnoughGoldText);
	}
}
