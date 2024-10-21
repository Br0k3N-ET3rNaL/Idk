// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/SettingsWidget.h"

#include "Idk/UI/AudioSettingsWidget.h"
#include "Idk/UI/CommonButton.h"
#include "Idk/UI/GraphicsSettingsWidget.h"
#include "Idk/UI/InputSettingsWidget.h"
#include "Idk/UI/SettingsSubMenuWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/WidgetSwitcher.h>
#include <GameFramework/GameUserSettings.h>
#include <Templates/Casts.h>

void USettingsWidget::Init()
{
	UserSettings = UGameUserSettings::GetGameUserSettings();

	UserSettings->ApplySettings(true);

	// Initialize sub-menus
	GraphicsSettings->Init(*UserSettings);
	InputSettings->Init();
	AudioSettings->Init();
}

void USettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GraphicsButton->OnClicked().AddUObject(this, &USettingsWidget::OnGraphicsButtonClicked);
	InputButton->OnClicked().AddUObject(this, &USettingsWidget::OnInputButtonClicked);
	AudioButton->OnClicked().AddUObject(this, &USettingsWidget::OnAudioButtonClicked);

	ApplyButton->OnClicked().AddUObject(this, &USettingsWidget::OnApplyButtonClicked);
	RevertButton->OnClicked().AddUObject(this, &USettingsWidget::OnRevertButtonClicked);
	ResetButton->OnClicked().AddUObject(this, &USettingsWidget::OnResetButtonClicked);
	ExitButton->OnClicked().AddUObject(this, &USettingsWidget::OnExitButtonClicked);
}

USettingsSubMenuWidget& USettingsWidget::GetActiveSubMenu() const
{
	return *CastChecked<USettingsSubMenuWidget>(WidgetSwitcher->GetActiveWidget());
}

void USettingsWidget::SetActiveSubMenu(USettingsSubMenuWidget& NewSubMenu)
{
	USettingsSubMenuWidget& CurrentSubMenu = GetActiveSubMenu();

	if (&CurrentSubMenu != &NewSubMenu)
	{
		// Revert any unsaved changes
		CurrentSubMenu.RevertToSavedValues();

		WidgetSwitcher->SetActiveWidget(&NewSubMenu);
	}
}

void USettingsWidget::OnGraphicsButtonClicked()
{
	SetActiveSubMenu(*GraphicsSettings);
}

void USettingsWidget::OnInputButtonClicked()
{
	SetActiveSubMenu(*InputSettings);
}

void USettingsWidget::OnAudioButtonClicked()
{
	SetActiveSubMenu(*AudioSettings);
}

void USettingsWidget::OnApplyButtonClicked()
{
	GetActiveSubMenu().ApplySettings();
}

void USettingsWidget::OnRevertButtonClicked()
{
	GetActiveSubMenu().RevertToSavedValues();
}

void USettingsWidget::OnResetButtonClicked()
{
	GetActiveSubMenu().ResetToDefaults();
}

void USettingsWidget::OnExitButtonClicked()
{
	GetActiveSubMenu().RevertToSavedValues();

	CloseSettingsDelegate.ExecuteIfBound();
}
