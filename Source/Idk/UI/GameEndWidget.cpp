// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GameEndWidget.h"

#include "Idk/IdkGameInstance.h"
#include "Idk/UI/CommonButton.h"
#include <Blueprint/UserWidget.h>
#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/UObjectGlobals.h>

UGameEndWidget::UGameEndWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WinMessage = FText::FromString(TEXT("Victory!"));
	LoseMessage = FText::FromString(TEXT("Defeat"));
}

void UGameEndWidget::OnGameEnd(const bool bWon)
{
	UIdkGameInstance::GetGameInstance(GetWorld())->ClearMidRunSave();

	if (bWon)
	{
		GameEndText->SetText(WinMessage);
	}
	else
	{
		GameEndText->SetText(LoseMessage);
	}
}

void UGameEndWidget::NativeOnInitialized()
{
	MainMenuButton->OnClicked().AddUObject(this, &UGameEndWidget::OnMainMenuButtonClicked);
	ExitButton->OnClicked().AddUObject(this, &UGameEndWidget::OnExitButtonClicked);
}

void UGameEndWidget::OnMainMenuButtonClicked()
{
	UIdkGameInstance::GetGameInstance(GetWorld())->RequestExitToMenu();
}

void UGameEndWidget::OnExitButtonClicked()
{
	UIdkGameInstance::GetGameInstance(GetWorld())->RequestExit();
}
