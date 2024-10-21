// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "PauseMenuWidget.generated.h"

class UCommonButton;
class USettingsWidget;

/** Widget that allows the player to return to the main menu, exit the game, or adjust settings. */
UCLASS(Abstract)
class UPauseMenuWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Initialize the pause menu widget. */
	void Init();

	/** Close the settings widget. */
	void CloseSettings();

	/** Delegate called to closed the pause menu. */
	FSimpleDelegate ClosePauseMenuDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Event called when the resume button is clicked. */
	void OnResumeButtonClicked() const;

	/** Event called when the return to main menu button is clicked. */
	void OnMainMenuButtonClicked();

	/** Event called when the settings button is clicked. */
	void OnSettingsButtonClicked();

	/** Event called when the exit game button is clicked. */
	void OnExitButtonClicked();

	/** Prevent buttons from being clicked. */
	void DisableButtons();

	/** Button to close the menu and unpause the game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ResumeButton;

	/** Button to return to the main menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> MainMenuButton;

	/** Button to open the settings menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> SettingsButton;

	/** Button to exit the game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ExitButton;

	/** Widget that allows to player to change settings. @see USettingsWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsWidget> SettingsWidget;

};
