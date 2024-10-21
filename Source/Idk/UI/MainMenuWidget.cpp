// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/MainMenuWidget.h"

#include "Idk/UI/CommonButton.h"
#include "Idk/UI/SettingsWidget.h"
#include <Components/SlateWrapperTypes.h>
#include <Components/WidgetSwitcher.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Misc/AssertionMacros.h>

void UMainMenuWidget::Init()
{
	check(SettingsWidget);

	SettingsWidget->Init();
}

void UMainMenuWidget::SetCanContinue(const bool bCanContinue)
{
	const ESlateVisibility ContinueVisibility = (bCanContinue) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	ContinueButton->SetVisibility(ContinueVisibility);
}

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ContinueButton->OnClicked().AddUObject(this, &UMainMenuWidget::OnContinueButtonClicked);
	PlayButton->OnClicked().AddUObject(this, &UMainMenuWidget::OnPlayButtonClicked);
	SettingsButton->OnClicked().AddUObject(this, &UMainMenuWidget::OnSettingsButtonClicked);
	ExitButton->OnClicked().AddUObject(this, &UMainMenuWidget::OnExitButtonClicked);

	// Switch back to the main menu when the settings widget is closed
	SettingsWidget->CloseSettingsDelegate.BindLambda([&]()
		{
			WidgetSwitcher->SetActiveWidget(MainMenu);
		});
}

void UMainMenuWidget::OnContinueButtonClicked() const
{
	OnContinueClickedDelegate.ExecuteIfBound();
}

void UMainMenuWidget::OnPlayButtonClicked() const
{
	OnPlayClickedDelegate.ExecuteIfBound();
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	WidgetSwitcher->SetActiveWidget(SettingsWidget);
}

void UMainMenuWidget::OnExitButtonClicked() const
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Type::Quit, false);
}
