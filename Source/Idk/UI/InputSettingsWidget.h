// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/UI/SettingsSubMenuWidget.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "InputSettingsWidget.generated.h"

class UEnhancedInputUserSettings;
class UInputBindingRowWidget;
class UScrollBox;

/** Widget that displays input related settings. */
UCLASS(Abstract)
class UInputSettingsWidget final : public USettingsSubMenuWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin USettingsSubMenuWidget Interface.
	virtual void ApplySettings() override;
	virtual void ResetToDefaults() override;
	virtual void RevertToSavedValues() override;
	//~ End USettingsSubMenuWidget Interface

	/** Initialize the input settings widget. */
	void Init();

private:
	/**
	 * Populate the widget with saved bindings.
	 * 
	 * @param bCreateRows	Whether to create widgets for each row or use existing. 
	 */
	void LoadSavedBindings(const bool bCreateRows = false);

	/** Scroll box containing widgets for each bindable input. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	/** Class for widgets that represent individual input bindings. @see UInputBindingRowWidget */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UInputBindingRowWidget> InputBindingRowClass;

	/** Saved input settings. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UEnhancedInputUserSettings> InputSettings;

};
