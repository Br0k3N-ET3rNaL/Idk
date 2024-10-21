// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/PersistentLevelScriptActor.h"

#include "Idk/CharacterSelectLevelScriptActor.h"
#include "Idk/GameSystems/MapManager.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/MidRunSaveGame.h"
#include "Idk/UI/LoadingScreenWidget.h"
#include <Blueprint/UserWidget.h>
#include <Containers/Map.h>
#include <CoreGlobals.h>
#include <Delegates/Delegate.h>
#include <Engine/LevelStreamingDynamic.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include <HAL/Platform.h>
#include <Kismet/GameplayStatics.h>
#include <Logging/LogMacros.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>
#include <VoxelTools/VoxelBlueprintLibrary.h>

void APersistentLevelScriptActor::SwitchToCharacterSelect()
{
	check(CharacterSelectStreaming->IsLevelLoaded());

	CastChecked<ACharacterSelectLevelScriptActor>(CharacterSelectStreaming->GetLoadedLevel()->GetLevelScriptActor())->Init();
}

void APersistentLevelScriptActor::LoadFromSave(const UMidRunSaveGame& InSave)
{
	Save = &InSave;

	GameInstance->SetPlayerClass(Save->GetPlayerClassEnum());

	LoadMainLevel(Save->GetPlayerClass());
}

void APersistentLevelScriptActor::LoadMainLevel(TSubclassOf<AIdkPlayerCharacter> InPlayerClass)
{
	check(InPlayerClass);

	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	Controller->DisableInput(Controller);

	LoadingScreenWidget =
		CreateWidget<ULoadingScreenWidget>(Controller, LoadingScreenWidgetClass.LoadSynchronous());

	LoadingScreenWidget->AddToViewport(1);

	if (Save)
	{
		GameInstance->CloseMainMenu();
	}

	GameInstance->SavePlayerClass(InPlayerClass);

	PlayerClass = InPlayerClass;

	CharacterSelectStreaming->OnLevelHidden.AddDynamic(this, &APersistentLevelScriptActor::OnCharacterSelectUnloaded);
	CharacterSelectStreaming->SetIsRequestingUnloadAndRemoval(true);
}

void APersistentLevelScriptActor::UnloadMainLevel()
{
	if (MapManager)
	{
		MapManager->Destroy();
	}

	MainLevelLightingStreaming->OnLevelHidden.AddDynamic(this, &APersistentLevelScriptActor::OnMainLevelLightingUnloaded);
	MainLevelLightingStreaming->SetIsRequestingUnloadAndRemoval(true);
}

void APersistentLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = UIdkGameInstance::GetGameInstance(GetWorld());

	check(GameInstance);

	GameInstance->OnBeginPlay();

	check(!CharacterSelectLevel.IsNull());
	check(!MainLevelLighting.IsNull());
	check(!MapManagerClass.IsNull());

	UVoxelBlueprintLibrary::CreateWorldVoxelThreadPool(GetWorld(), TMap<EVoxelTaskType, int32>(), TMap<EVoxelTaskType, int32>());

	bool bLoadSuccess = false;

	CharacterSelectStreaming = 
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), CharacterSelectLevel, FTransform(), bLoadSuccess);

	CharacterSelectStreaming->OnLevelLoaded.AddDynamic(this, &APersistentLevelScriptActor::OnCharacterSelectLoaded);
}

void APersistentLevelScriptActor::OnPlayerSpawned()
{
	UE_LOG(LogTemp, Warning, TEXT("Player spawned."));

	LoadingScreenWidget->RemoveFromParent();
}

void APersistentLevelScriptActor::OnCharacterSelectLoaded()
{
	CharacterSelectStreaming->OnLevelLoaded.RemoveDynamic(this, &APersistentLevelScriptActor::OnCharacterSelectLoaded);

	ACharacterSelectLevelScriptActor* CharSelectLevelActor
		= CastChecked<ACharacterSelectLevelScriptActor>(CharacterSelectStreaming->GetLoadedLevel()->GetLevelScriptActor());

	CharSelectLevelActor->LoadMainLevelDelegate.BindUObject(this, &APersistentLevelScriptActor::LoadMainLevel);
}

void APersistentLevelScriptActor::OnCharacterSelectUnloaded()
{
	CharacterSelectStreaming->OnLevelHidden.RemoveDynamic(this, &APersistentLevelScriptActor::OnCharacterSelectUnloaded);

	bool bLoadSuccess = false;

	MainLevelLightingStreaming = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), MainLevelLighting, FTransform(), bLoadSuccess);

	MainLevelLightingStreaming->OnLevelLoaded.AddDynamic(this, &APersistentLevelScriptActor::OnMainLevelLightingLoaded);
}

void APersistentLevelScriptActor::OnMainLevelLightingLoaded()
{
	MainLevelLightingStreaming->OnLevelLoaded.RemoveDynamic(this, &APersistentLevelScriptActor::OnMainLevelLightingLoaded);

	MapManager = GetWorld()->SpawnActorDeferred<AMapManager>(MapManagerClass.LoadSynchronous(), FTransform());

	MapManager->SetPlayerClass(PlayerClass.LoadSynchronous());

	if (Save)
	{
		MapManager->LoadFromSave(*Save);

		Save = nullptr;
	}

	MapManager->OnPlayerSpawnedDelegate.BindUObject(this, &APersistentLevelScriptActor::OnPlayerSpawned);
	
	UGameplayStatics::FinishSpawningActor(MapManager, FTransform());
}

void APersistentLevelScriptActor::OnMainLevelLightingUnloaded()
{
	MainLevelLightingStreaming->OnLevelHidden.RemoveDynamic(this, &APersistentLevelScriptActor::OnMainLevelLightingUnloaded);

	bool bLoadSuccess = false;

	CharacterSelectStreaming =
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), CharacterSelectLevel, FTransform(), bLoadSuccess);

	CharacterSelectStreaming->OnLevelLoaded.AddDynamic(this, &APersistentLevelScriptActor::OnCharacterSelectLoaded);
}
