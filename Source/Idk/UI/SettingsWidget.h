// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "SettingsWidget.generated.h"

class UAudioSettingsWidget;
class UCommonButton;
class UGameUserSettings;
class UGraphicsSettingsWidget;
class UInputSettingsWidget;
class USettingsSubMenuWidget;
class UWidgetSwitcher;

/** Widget that allows to player to change settings. */
UCLASS(Abstract)
class USettingsWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Initialize the settings widget. */
	void Init();

	/** Delegate called to close the settings menu. */
	FSimpleDelegate CloseSettingsDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Get the currently displayed sub-menu. */
	UE_NODISCARD USettingsSubMenuWidget& GetActiveSubMenu() const;

	/** Set the currently displayed sub-menu. */
	void SetActiveSubMenu(USettingsSubMenuWidget& NewSubMenu);

	/** Event called when the button for graphics settings is clicked. */
	void OnGraphicsButtonClicked();

	/** Event called when the button for input settings is clicked. */
	void OnInputButtonClicked();

	/** Event called when the button for audio settings is clicked.  */
	void OnAudioButtonClicked();

	/** Event called when the apply button is clicked. */
	void OnApplyButtonClicked();

	/** Event called when the revert button is clicked. */
	void OnRevertButtonClicked();

	/** Event called when the reset button is clicked. */
	void OnResetButtonClicked();

	/** Event called when the exit button is clicked. */
	void OnExitButtonClicked();

	/** Button to show graphics settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> GraphicsButton;

	/** Button to show input settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> InputButton;

	/** Button to show audio settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> AudioButton;

	/** Widget that switches between the different sub-menus. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/** Widget that displays graphics related settings. @see UGraphicsSettingsWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGraphicsSettingsWidget> GraphicsSettings;

	/** Widget that displays input related settings. @see UInputSettingsWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputSettingsWidget> InputSettings;

	/** Widget that displays audio related settings. @see UAudioSettingsWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAudioSettingsWidget> AudioSettings;

	/** Button that saves and applies current settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ApplyButton;

	/** Button that reverts setting to saved values. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> RevertButton;

	/** Button that resets settings to their default values. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ResetButton;

	/** Button that closes the settings widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ExitButton;

	/** Used to save and load game related settings. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UGameUserSettings> UserSettings;

};
