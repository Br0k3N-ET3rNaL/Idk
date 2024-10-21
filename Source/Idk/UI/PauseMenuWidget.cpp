// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/PauseMenuWidget.h"

#include "Idk/IdkGameInstance.h"
#include "Idk/UI/CommonButton.h"
#include "Idk/UI/SettingsWidget.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Components/SlateWrapperTypes.h>
#include <GenericPlatform/ICursor.h>
#include <Misc/AssertionMacros.h>

void UPauseMenuWidget::Init()
{
	check(SettingsWidget);

	SettingsWidget->Init();
}

void UPauseMenuWidget::CloseSettings()
{
	SettingsWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UPauseMenuWidget::NativeOnInitialized()
{
	ResumeButton->OnClicked().AddUObject(this, &UPauseMenuWidget::OnResumeButtonClicked);
	MainMenuButton->OnClicked().AddUObject(this, &UPauseMenuWidget::OnMainMenuButtonClicked);
	SettingsButton->OnClicked().AddUObject(this, &UPauseMenuWidget::OnSettingsButtonClicked);
	ExitButton->OnClicked().AddUObject(this, &UPauseMenuWidget::OnExitButtonClicked);

	SettingsWidget->SetVisibility(ESlateVisibility::Hidden);

	SettingsWidget->CloseSettingsDelegate.BindLambda([this]()
		{
			SettingsWidget->SetVisibility(ESlateVisibility::Hidden);
		});
}

void UPauseMenuWidget::OnResumeButtonClicked() const
{
	check(ClosePauseMenuDelegate.IsBound());

	ClosePauseMenuDelegate.Execute();
}

void UPauseMenuWidget::OnMainMenuButtonClicked()
{
	DisableButtons();

	UWidgetBlueprintLibrary::SetFocusToGameViewport();

	// Set cursor to the cursor used for waiting
	SetCursor(EMouseCursor::Custom);

	UIdkGameInstance::GetGameInstance(GetWorld())->RequestExitToMenu([this]()
		{
			// Revert cursor to default
			SetCursor(EMouseCursor::Default);
		});
}

void UPauseMenuWidget::OnSettingsButtonClicked()
{
	SettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenuWidget::OnExitButtonClicked()
{
	DisableButtons();

	UWidgetBlueprintLibrary::SetFocusToGameViewport();

	// Set cursor to the cursor used for waiting
	SetCursor(EMouseCursor::Custom);

	UIdkGameInstance::GetGameInstance(GetWorld())->RequestExit();
}

void UPauseMenuWidget::DisableButtons()
{
	ResumeButton->SetIsEnabled(false);
	MainMenuButton->SetIsEnabled(false);
	SettingsButton->SetIsEnabled(false);
	ExitButton->SetIsEnabled(false);
}
