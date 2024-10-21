// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemGenerator.h"
#include "Idk/GameSystems/PedestalSelector.h"
#include "Idk/GameSystems/RewardPedestalSelector.h"
#include "Idk/GameSystems/RoomManagers/ArenaMeshManager.h"
#include "Idk/GameSystems/RoomSpawnInfo.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/UI/PedestalDisplayInfo.h"
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <Engine/World.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult AArenaRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (RewardPedestalSelectorClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AArenaRoomManager: Reward pedestal selector class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (MeshManagerClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AArenaRoomManager: Mesh manager class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AArenaRoomManager::Destroyed()
{
	if (MeshManager)
	{
		MeshManager->Destroy();
	}

	if (RewardPedestalSelector)
	{
		RewardPedestalSelector->Destroy();
	}

	Super::Destroyed();
}

void AArenaRoomManager::Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents)
{
	Super::Init(RoomSpawnInfo, BoxExtents);

	check(!MeshManagerClass.IsNull());
	check(!RewardPedestalSelectorClass.IsNull());

	MeshManager = GetWorld()->SpawnActor<AArenaMeshManager>(MeshManagerClass.LoadSynchronous());
	MeshManager->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	MeshManager->Init(RoomSpawnInfo.GetGapAngles());

	RewardLevel = Level;
}

void AArenaRoomManager::EnableSpawnPortal(const bool bInLeftPortal)
{
	bSpawnPortal = true;
	bLeftPortal = bInLeftPortal;
}

FTransform AArenaRoomManager::GetPortalSource() const
{
	return MeshManager->GetGapTransform(true, bLeftPortal);
}

FTransform AArenaRoomManager::GetPortalDestination(const bool bLeft) const
{
	return MeshManager->GetGapTransform(false, !bLeft);
}

void AArenaRoomManager::OnEnabled()
{
	Super::OnEnabled();

	// Open the back doors
	MeshManager->InitDoors();
	MeshManager->SetBackDoorsOpen(true);
}

void AArenaRoomManager::OnDisabled()
{
	Super::OnDisabled();

	// Close the front doors
	MeshManager->SetFrontDoorsOpen(false);

	if (RewardPedestalSelector)
	{
		RewardPedestalSelector->Destroy();
	}
}

void AArenaRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	// Close the back doors
	MeshManager->SetBackDoorsOpen(false);
}

void AArenaRoomManager::OnCompletion()
{
	Super::OnCompletion();

	// Close the back doors
	MeshManager->SetBackDoorsOpen(false);

	// Open the front doors
	MeshManager->SetFrontDoorsOpen(true);

	if (bSpawnPortal)
	{
		SpawnPortal();
	}
}

void AArenaRoomManager::OnPedestalSelected(const uint8 Index)
{
	RemoveItemDelegates[Index].ExecuteIfBound();

	check(IsValid(Player));
	Player->AddItemToInventory(*GeneratedItems[Index]);

	Complete();
}

void AArenaRoomManager::OnAllEnemiesKilled()
{
	const UIdkGameInstance* GameInstance = CastChecked<UIdkGameInstance>(GetGameInstance());
	UItemGenerator& ItemGenerator = GameInstance->GetItemGenerator();

	RewardPedestalSelector = GetWorld()->SpawnActor<ARewardPedestalSelector>(
		RewardPedestalSelectorClass.LoadSynchronous());
	RewardPedestalSelector->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ItemGenerator.GenerateItems(Rng->GenerateSeed(), 
		RewardPedestalSelector->GetNumPedestals(), Level, GeneratedItems, RemoveItemDelegates);

	TArray<FPedestalDisplayInfo> PedestalDisplayInfo;

	for (const UItem* Item : GeneratedItems)
	{
		PedestalDisplayInfo.Add(FPedestalDisplayInfo(Item->GetDisplayInfo()));
	}

	RewardPedestalSelector->Init(PedestalDisplayInfo);
	RewardPedestalSelector->PedestalSelectedDelegate.BindUObject(this, &AArenaRoomManager::OnPedestalSelected);
}

void AArenaRoomManager::SpawnPortal()
{
	check(bSpawnPortal);
	check(GetPortalDestDelegate.IsBound());

	const FTransform Source = GetPortalSource();
	const FTransform Destination = GetPortalDestDelegate.Execute();

	Super::SpawnPortal(Source, Destination);
}
