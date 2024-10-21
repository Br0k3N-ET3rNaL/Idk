// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GameEndWidget.generated.h"

class FObjectInitializer;
class UCommonButton;
class UTextBlock;

/** 
 * Widget shown when the game ends.
 * Shown message depends on whether the player won or lost.
 * Allows the player to return to the main menu or exit the game. 
 */
UCLASS(Abstract)
class UGameEndWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameEndWidget(const FObjectInitializer& ObjectInitializer);

	/**
	 * Event called when the game ends.
	 * 
	 * @param bWon	Whether the player won or lost. 
	 */
	void OnGameEnd(const bool bWon);

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Event called when the main menu button is clicked. */
	void OnMainMenuButtonClicked();

	/** Event called when the exit button is clicked. */
	void OnExitButtonClicked();

	/** Displays the game over message. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameEndText;

	/** Button to return to the main menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> MainMenuButton;

	/** Button to exit the game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ExitButton;

	/** Text to show when the player wins. */
	UPROPERTY(EditDefaultsOnly)
	FText WinMessage;

	/** Text to show when the player loses. */
	UPROPERTY(EditDefaultsOnly)
	FText LoseMessage;

};
