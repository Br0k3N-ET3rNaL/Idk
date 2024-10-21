// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>

#include "SettingsSubMenuWidget.generated.h"

/** Base class for widgets representing a tab in the settings menu. */
UCLASS(Abstract)
class USettingsSubMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Save and apply the current settings. */
	virtual void ApplySettings() PURE_VIRTUAL(&USettingsSubMenuWidget::ApplySettings, ;);

	/** Reset settings to their default value. */
	virtual void ResetToDefaults() PURE_VIRTUAL(&USettingsSubMenuWidget::ResetToDefaults, ;);

	/** Revert settings to saved values. */
	virtual void RevertToSavedValues() PURE_VIRTUAL(&USettingsSubMenuWidget::RevertToSavedValues, ;);
};
