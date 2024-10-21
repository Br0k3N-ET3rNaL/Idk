// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Player/IdkPlayerController.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/UI/HUDWidget.h"
#include <Blueprint/UserWidget.h>
#include <Engine/EngineBaseTypes.h>
#include <Engine/LocalPlayer.h>
#include <EnhancedInputSubsystems.h>
#include <GameFramework/Pawn.h>
#include <GenericPlatform/ICursor.h>
#include <InputMappingContext.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UserSettings/EnhancedInputUserSettings.h>

AIdkPlayerController::AIdkPlayerController()
{
	// Setup the input modes
	GameplayInputMode.SetConsumeCaptureMouseDown(true);
	MenuInputMode.SetHideCursorDuringCapture(false);
	MenuInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
}

UHUDWidget& AIdkPlayerController::GetHUDWidget()
{
	return *HUDWidget;
}

void AIdkPlayerController::SetInputModeToGameplay(const bool bChangeInputContext)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	SetInputMode(GameplayInputMode);

	SetShowMouseCursor(false);

	if (bChangeInputContext)
	{
		Subsystem->RemoveMappingContext(MenuInputMapping.LoadSynchronous());
		Subsystem->AddMappingContext(DefaultInputMapping.LoadSynchronous(), 0);
	}
}

void AIdkPlayerController::SetInputModeToMenu(const bool bChangeInputContext)
{
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	SetInputMode(MenuInputMode);

	SetShowMouseCursor(true);

	if (bChangeInputContext)
	{
		InputSubsystem->RemoveMappingContext(DefaultInputMapping.LoadSynchronous());
		InputSubsystem->AddMappingContext(MenuInputMapping.LoadSynchronous(), 0);
	}
}

void AIdkPlayerController::ToggleMouseVisibility()
{
	if (ShouldShowMouseCursor())
	{
		SetInputModeToGameplay(false);
	}
	else
	{
		SetInputModeToMenu(false);
	}
}

UEnhancedInputUserSettings& AIdkPlayerController::GetInputSettings()
{
	if (!InputSettings)
	{
		LoadOrCreateInputSettings();
	}

	return *InputSettings;
}

void AIdkPlayerController::WaitForExit()
{
	DisableInput(this);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Subsystem->RemoveMappingContext(MenuInputMapping.LoadSynchronous());
}

void AIdkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(!MenuInputMapping.IsNull());
	check(!DefaultInputMapping.IsNull());

	if (!InputSettings)
	{
		LoadOrCreateInputSettings();
	}

	UUserWidget* WaitingMouseCursorWidget
		= CreateWidget(this, WaitingMouseCursorWidgetClass.LoadSynchronous());

	SetMouseCursorWidget(EMouseCursor::Custom, WaitingMouseCursorWidget);
}

void AIdkPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	check(!HUDWidgetClass.IsNull());

	if (AIdkPlayerCharacter* PlayerChar = Cast<AIdkPlayerCharacter>(InPawn))
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass.LoadSynchronous());

		check(HUDWidget);

		HUDWidget->Init();
		HUDWidget->AddToViewport();

		PlayerChar->OnPossessed(this);

		FPlayerWidgets PlayerWidgets;

		HUDWidget->GetPlayerWidgets(PlayerWidgets);

		UHealthAndStatusBarWidget& HealthAndStatusBar = HUDWidget->GetHealthAndStatusBarWidget();

		PlayerChar->SetWidgets(HealthAndStatusBar, PlayerWidgets);

		SetInputModeToGameplay();

		PlayerChar->DisableInput(this);
	}
}

void AIdkPlayerController::OnUnPossess()
{
	if (HUDWidget && HUDWidget->IsInViewport())
	{
		HUDWidget->RemoveFromParent();

		EnableInput(this);
	}
}

void AIdkPlayerController::LoadOrCreateInputSettings()
{
	InputSettings = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();

	InputSettings->RegisterInputMappingContext(DefaultInputMapping.LoadSynchronous());
	InputSettings->RegisterInputMappingContext(MenuInputMapping.LoadSynchronous());
	InputSettings->RegisterInputMappingContext(MapInputMapping.LoadSynchronous());
}
