// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/UI/SettingsSubMenuWidget.h"
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <GenericPlatform/GenericWindow.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/Crc.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GraphicsSettingsWidget.generated.h"

class UComboBoxString;
class UGameUserSettings;

/** Represents the resolution of the game window. */
USTRUCT()
struct FScreenResolution final
{
	GENERATED_BODY()

public:
	FScreenResolution(const int32 Width = 1920, const int32 Height = 1080);
	FScreenResolution(const FIntPoint& IntPoint);

	UE_NODISCARD operator FIntPoint() const;
	UE_NODISCARD bool operator==(const FScreenResolution& Other) const;
	UE_NODISCARD bool operator!=(const FScreenResolution& Other) const;
	UE_NODISCARD bool operator<(const FScreenResolution& Other) const;

	/** Get the width. */
	UE_NODISCARD int32 GetWidth() const;

	/** Get the height. */
	UE_NODISCARD int32 GetHeight() const;

	/** Get the string representation of the resolution. */
	UE_NODISCARD const FString& ToString() const;

	/** Get a resolution from a string. */
	UE_NODISCARD static FScreenResolution FromString(const FString& String);

private:
	/** Width of the window. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "800", ClampMax = "5120"))
	int32 Width = 1920;

	/** Height of the window. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "720", ClampMax = "2160"))
	int32 Height = 1080;

	/** String representing the resolution. */
	mutable FString String;

	/** Delimiter used for strings. */
	static const FString Delimiter;

};

FORCEINLINE uint32 GetTypeHash(const FScreenResolution& Resolution)
{
	return FCrc::MemCrc32(&Resolution, sizeof(FScreenResolution));
}

/** Widget that displays graphics related settings. */
UCLASS(Abstract)
class UGraphicsSettingsWidget final : public USettingsSubMenuWidget
{
	GENERATED_BODY()
	
public:
	UGraphicsSettingsWidget();

	//~ Begin USettingsSubMenuWidget Interface.
	virtual void ApplySettings() override;
	virtual void ResetToDefaults() override;
	virtual void RevertToSavedValues() override;
	//~ End USettingsSubMenuWidget Interface

	/**
	 * Initialize the graphics settings widget.
	 * 
	 * @param InUserSettings 
	 */
	void Init(UGameUserSettings& InUserSettings);

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Add default options to the resolution selector. */
	void AddResolutionOptions();

	/** Initialize selectors from saved values. */
	void InitValuesFromUserSettings();

	/** Selector for window resolution. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionSelector;

	/** Selector for window mode. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeSelector;

	/** Selector for overall graphics quality. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> QualitySelector;

	/** Set of selectable resolution options. */
	UPROPERTY(EditDefaultsOnly, meta = (ShowOnlyInnerProperties))
	TSet<FScreenResolution> Resolutions;

	/** Saved user settings. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UGameUserSettings> UserSettings;

	/** Maps the string representation of a window mode to the window mode. */
	TMap<FString, EWindowMode::Type> WindowModeStringMap;
	
	/** Number of quality presets. */
	static const int32 NumQualityPresets = 4;

};
