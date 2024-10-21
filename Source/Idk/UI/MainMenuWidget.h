// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "MainMenuWidget.generated.h"

class UCommonButton;
class USettingsWidget;
class UWidget;
class UWidgetSwitcher;

/** Widget for the game's main menu. */
UCLASS(Abstract)
class UMainMenuWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Initialize the main menu widget. */
	void Init();

	/** Set whether the player can continue from a saved game. */
	void SetCanContinue(const bool bCanContinue);

	/** Delegate called when the play button is clicked. */
	FSimpleDelegate OnPlayClickedDelegate;

	/** Delegate called when the continue button is clicked. */
	FSimpleDelegate OnContinueClickedDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Event called when the continue button is clicked. */
	void OnContinueButtonClicked() const;

	/** Event called when the play button is clicked.  */
	void OnPlayButtonClicked() const;

	/** Event called when the settings button is clicked.  */
	void OnSettingsButtonClicked();

	/** Event called when the exit button is clicked.  */
	void OnExitButtonClicked() const;

	/** Used to switch between the main menu and the settings menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/** Group of widgets representing the main menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainMenu;

	/** Button to continue from a saved game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ContinueButton;

	/** Button to start a new game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> PlayButton;

	/** Button to view/change settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> SettingsButton;

	/** Button to exit the game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ExitButton;

	/** Widget that allows to player to change settings. @see USettingsWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsWidget> SettingsWidget;
};
