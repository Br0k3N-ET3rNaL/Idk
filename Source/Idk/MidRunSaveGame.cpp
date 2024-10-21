// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/MidRunSaveGame.h"

#include "Idk/Character/PlayerClasses.h"
#include "Idk/Character/PlayerSaveData.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <GameFramework/SaveGame.h>
#include <HAL/Platform.h>
#include <Kismet/GameplayStatics.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>

const FString UMidRunSaveGame::SlotName = TEXT("MidRun");
const int32 UMidRunSaveGame::UserIndex = 0;

void UMidRunSaveGame::SetSeed(const int32 InSeed)
{
	Seed = InSeed;
}

void UMidRunSaveGame::SetPlayerClass(const TSubclassOf<AIdkPlayerCharacter> InPlayerClass)
{
	PlayerClass = InPlayerClass;
}

void UMidRunSaveGame::SetPlayerClassEnum(const EPlayerClass InPlayerClassEnum)
{
	PlayerClassEnum = InPlayerClassEnum;
}

void UMidRunSaveGame::UpdatePlayerProgress(const EPathTaken PathTaken)
{
	PlayerProgress.Add(PathTaken);
}

void UMidRunSaveGame::SaveRemovedItem(const FItemSaveData& ItemSaveData)
{
	RemovedItemSaveData.Emplace(ItemSaveData);
}

void UMidRunSaveGame::SetPlayerSaveData(const FPlayerSaveData& InPlayerSaveData)
{
	PlayerSaveData = InPlayerSaveData;
}

int32 UMidRunSaveGame::GetSeed() const
{
	return Seed;
}

const TArray<EPathTaken>& UMidRunSaveGame::GetPlayerProgress() const
{
	return PlayerProgress;
}

TSubclassOf<AIdkPlayerCharacter> UMidRunSaveGame::GetPlayerClass() const
{
	return PlayerClass;
}

EPlayerClass UMidRunSaveGame::GetPlayerClassEnum() const
{
	return PlayerClassEnum;
}

const TArray<FItemSaveData>& UMidRunSaveGame::GetRemovedItemSaveData() const
{
	return RemovedItemSaveData;
}

const FPlayerSaveData& UMidRunSaveGame::GetPlayerSaveData() const
{
	return PlayerSaveData;
}

void UMidRunSaveGame::SetSaveAsComplete()
{
	bComplete = true;

	SaveVersion = CurrentSaveVersion;
}

bool UMidRunSaveGame::IsValid() const
{
	return SaveVersion == CurrentSaveVersion && PlayerClass && PlayerClassEnum != EPlayerClass::None;
}

bool UMidRunSaveGame::IsComplete() const
{
	return bComplete;
}

bool UMidRunSaveGame::IsSaving() const
{
	return bSaving;
}

void UMidRunSaveGame::SaveAsync()
{
	FAsyncSaveGameToSlotDelegate SaveCompleteDelegate 
		= FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UMidRunSaveGame::OnSaveComplete);

	bSaving = true;

	UGameplayStatics::AsyncSaveGameToSlot(this, SlotName, UserIndex, SaveCompleteDelegate);
}

UMidRunSaveGame& UMidRunSaveGame::CreateMidRunSave()
{
	return *CastChecked<UMidRunSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
}

UMidRunSaveGame* UMidRunSaveGame::LoadMidRunSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UMidRunSaveGame* ExistingSave = CastChecked<UMidRunSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));

		if (ExistingSave->IsValid() && ExistingSave->IsComplete())
		{
			return ExistingSave;
		}
	
		DeleteMidRunSave();
	}

	return nullptr;
}

void UMidRunSaveGame::DeleteMidRunSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		verify(UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex));
	}
}

void UMidRunSaveGame::OnSaveComplete(const FString& InSlotName, const int32 InUserIndex, bool bSuccessful) const
{
	if (InSlotName.Equals(SlotName) && InUserIndex == UserIndex)
	{
		check(bSuccessful);

		bSaving = false;

		OnSaveCompleteDelegate.ExecuteIfBound();
	}
}
