// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/IdkGameInstance.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/Character/PlayerClasses.h"
#include "Idk/Character/PlayerSaveData.h"
#include "Idk/EffectSystem/StatusEffectTable.h"
#include "Idk/GameInstanceActor.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemGenerator.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include "Idk/MidRunSaveGame.h"
#include "Idk/PersistentLevelScriptActor.h"
#include "Idk/Player/IdkPlayerController.h"
#include "Idk/UI/HealthAndStatusBarWidget.h"
#include "Idk/UI/HUDWidget.h"
#include "Idk/UI/MainMenuWidget.h"
#include <Blueprint/UserWidget.h>
#include <Containers/Array.h>
#include <CoreGlobals.h>
#include <GameFramework/PlayerController.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Logging/LogMacros.h>
#include <Materials/MaterialInterface.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <NiagaraSystem.h>
#include <Particles/ParticleSystem.h>
#include <Templates/Casts.h>
#include <Templates/Function.h>
#include <Templates/SubclassOf.h>
#include <Templates/UnrealTemplate.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult UIdkGameInstance::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (ItemGeneratorClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Item generator class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (MainMenuWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Main menu widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (AbilityIndicatorMaterial.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Ability indicator material is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (DamageNumberParticleSystem.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Damage number particle system is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (SpawnVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Spawn VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (HealthAndStatusBarWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Health and status bar widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!DoorOpenCurve)
	{
		Context.AddError(FText::FromString(TEXT("Door open curve is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UIdkGameInstance::OnStart()
{
	check(!ItemGeneratorClass.IsNull());
	check(!MainMenuWidgetClass.IsNull());
	check(!AbilityIndicatorMaterial.IsNull());
	check(!DamageNumberParticleSystem.IsNull());
	check(!SpawnVFX.IsNull());
	check(!HealthAndStatusBarWidgetClass.IsNull());

	Super::OnStart();

	ItemGenerator = NewObject<UItemGenerator>(this, ItemGeneratorClass.LoadSynchronous());

	InitSaveGame();

	GameInstanceActor = GetWorld()->SpawnActor<AGameInstanceActor>();
}

UItemGenerator& UIdkGameInstance::GetItemGenerator() const
{
	check(ItemGenerator);

	return *ItemGenerator;
}

UMaterialInterface& UIdkGameInstance::GetAbilityIndicatorMaterial() const
{
	return *AbilityIndicatorMaterial.LoadSynchronous();
}

UNiagaraSystem& UIdkGameInstance::GetDamageNumberParticleSystem() const
{
	return *DamageNumberParticleSystem.LoadSynchronous();
}

UParticleSystem& UIdkGameInstance::GetSpawnVFX() const
{
	return *SpawnVFX.LoadSynchronous();
}

TSubclassOf<UHealthAndStatusBarWidget> UIdkGameInstance::GetHealthAndStatusBarWidgetClass() const
{
	return HealthAndStatusBarWidgetClass.LoadSynchronous();
}

UCurveFloat& UIdkGameInstance::GetDoorOpenCurve() const
{
	return *DoorOpenCurve;
}

void UIdkGameInstance::SetPlayerClass(const EPlayerClass PlayerClass)
{
	ItemGenerator->Init(PlayerClass);

	if (MidRunSave && MidRunSave->GetPlayerClassEnum() != PlayerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Saving player class enum"));

		MidRunSave->SetPlayerClassEnum(PlayerClass);
	}
}

void UIdkGameInstance::SetPlayer(AIdkPlayerCharacter& InPlayer)
{
	check(MidRunSave);

	Player = &InPlayer;

	Player->OnPlayerDeathDelegate.BindUObject(this, &UIdkGameInstance::OnPlayerDeath);

	ItemGenerator->GetItemTagCountsDelegate.BindUObject(Player, &AIdkPlayerCharacter::GetItemTagCounts);

	ShowTempMessageDelegate.BindUObject(&Cast<AIdkPlayerController>(Player->GetController())->GetHUDWidget()
		, &UHUDWidget::ShowTempMessage);

	StatusEffectTable.LoadSynchronous()->InitDescriptions(*Player->GetAttributeSystem());
	
	if (MidRunSave->IsComplete())
	{
		TArray<UItem*> SavedItems;

		ItemGenerator->GetSavedItems(MidRunSave->GetPlayerSaveData().GetSavedItems(), SavedItems);

		Player->RestoreSaveData(MidRunSave->GetPlayerSaveData(), SavedItems);

		ItemGenerator->RemoveSavedItems(MidRunSave->GetRemovedItemSaveData());
	}
	else
	{
		TArray<UItem*> SavedItems;

		// Add starter items for testing
		ItemGenerator->GetSavedItems(StarterItemData, SavedItems);
		Player->RestoreInventory(SavedItems);
		ItemGenerator->RemoveSavedItems(StarterItemData);

		Player->GenerateStartingAbility();
		
		SaveGame();

		MidRunSave->SetSaveAsComplete();
	}

	OnPlayerSetDelegate.Broadcast(Player);
}

AIdkPlayerCharacter* UIdkGameInstance::GetPlayer() const
{
	return Player;
}

void UIdkGameInstance::OnBeginPlay()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	MainMenuWidget 
		= CreateWidget<UMainMenuWidget>(Controller, MainMenuWidgetClass.LoadSynchronous());

	APersistentLevelScriptActor* LevelScriptActor 
		= CastChecked<APersistentLevelScriptActor>(GetWorld()->GetLevelScriptActor());

	MainMenuWidget->OnPlayClickedDelegate.BindLambda([&, LevelScriptActor]()
		{
			CreateNewMidRunSave();

			LevelScriptActor->SwitchToCharacterSelect();
		});
	
	MainMenuWidget->OnContinueClickedDelegate.BindLambda([&, LevelScriptActor]()
		{
			LevelScriptActor->LoadFromSave(*MidRunSave);
		});

	const bool bCanContinue = MidRunSave != nullptr && MidRunSave->IsValid();

	MainMenuWidget->Init();
	MainMenuWidget->SetCanContinue(bCanContinue);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenuWidget->GetCachedWidget());

	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(true);
	MainMenuWidget->AddToViewport();
}

void UIdkGameInstance::CloseMainMenu()
{
	MainMenuWidget->RemoveFromParent();
}

void UIdkGameInstance::OnPlayerDeath()
{
	ClearMidRunSave();

	ReturnToMainMenu();
}

void UIdkGameInstance::ReturnToMainMenu()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenuWidget->GetCachedWidget());

	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(true);
	
	InitSaveGame();

	const bool bCanContinue = MidRunSave != nullptr && MidRunSave->IsValid();

	MainMenuWidget->SetCanContinue(bCanContinue);
	MainMenuWidget->AddToViewport();

	if (IsValid(Player))
	{
		Player->Destroy();
	}
	
	Player = nullptr;

	ItemGenerator->Reset();

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	CastChecked<APersistentLevelScriptActor>(GetWorld()->GetLevelScriptActor())
		->UnloadMainLevel();
}

void UIdkGameInstance::OnLevelComplete()
{
	ClearMidRunSave();

	ReturnToMainMenu();
}

void UIdkGameInstance::CreateNewMidRunSave()
{
	MidRunSave = CastChecked<UMidRunSaveGame>(UGameplayStatics::CreateSaveGameObject(UMidRunSaveGame::StaticClass()));
}

void UIdkGameInstance::SaveMapSeed(const int32 Seed)
{
	UE_LOG(LogTemp, Warning, TEXT("Saving seed"));

	MidRunSave->SetSeed(Seed);
}

void UIdkGameInstance::SavePlayerClass(const TSubclassOf<AIdkPlayerCharacter> PlayerClass)
{
	UE_LOG(LogTemp, Warning, TEXT("Saving player class"));

	MidRunSave->SetPlayerClass(PlayerClass);
}

void UIdkGameInstance::UpdatePlayerProgress(const EPathTaken PathTaken)
{
	UE_LOG(LogTemp, Warning, TEXT("Saving player progress"));

	MidRunSave->UpdatePlayerProgress(PathTaken);

	SaveGame();
}

void UIdkGameInstance::SaveRemovedItem(const FItemSaveData& ItemSaveData)
{
	MidRunSave->SaveRemovedItem(ItemSaveData);
}

void UIdkGameInstance::ClearMidRunSave()
{
	UMidRunSaveGame::DeleteMidRunSave();

	MidRunSave = nullptr;
}

void UIdkGameInstance::OnSaveComplete()
{
	if (PreExitCallback)
	{
		PreExitCallback();

		PreExitCallback.Reset();
	}

	if (bExitAfterSaving)
	{
		ExitGame();
	}
	else if (bReturnToMenuAfterSaving)
	{
		ReturnToMainMenu();
	}
}

bool UIdkGameInstance::IsSaving() const
{
	return MidRunSave && MidRunSave->IsSaving();
}

void UIdkGameInstance::ShowTempMessage(const FText& Message) const
{
	ShowTempMessageDelegate.Execute(Message);
}

void UIdkGameInstance::RequestExit(TFunction<void(void)>&& Callback)
{
	if (Callback)
	{
		PreExitCallback = MoveTemp(Callback);
	}

	if (MidRunSave && MidRunSave->IsSaving())
	{
		CastChecked<AIdkPlayerController>(Player->GetController())->WaitForExit();

		bExitAfterSaving = true;
	}
	else
	{
		ExitGame();
	}
}

void UIdkGameInstance::RequestExitToMenu(TFunction<void(void)>&& Callback)
{
	if (Callback)
	{
		PreExitCallback = MoveTemp(Callback);
	}

	if (MidRunSave && MidRunSave->IsSaving())
	{
		CastChecked<AIdkPlayerController>(Player->GetController())->WaitForExit();

		bReturnToMenuAfterSaving = true;
	}
	else
	{
		ReturnToMainMenu();
	}
}

UIdkGameInstance* UIdkGameInstance::GetGameInstance(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);

	return Cast<UIdkGameInstance>(GameInstance);
}

void UIdkGameInstance::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Saving..."));

	check(Player);
	check(MidRunSave);

	MidRunSave->SetPlayerSaveData(Player->GetSaveData());

	MidRunSave->SaveAsync();
}

void UIdkGameInstance::InitSaveGame()
{
	MidRunSave = UMidRunSaveGame::LoadMidRunSave();
}

void UIdkGameInstance::ExitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Type::Quit, false);
}
