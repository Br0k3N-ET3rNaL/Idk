// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/UI/SettingsSubMenuWidget.h"
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AudioSettingsWidget.generated.h"

class UAudioUserSettings;
class USoundClass;
class UVolumeSlider;

/** Widget that displays audio related settings. */
UCLASS(Abstract)
class UAudioSettingsWidget final : public USettingsSubMenuWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin USettingsSubMenuWidget Interface.
	virtual void ApplySettings() override;
	virtual void ResetToDefaults() override;
	virtual void RevertToSavedValues() override;
	//~ End USettingsSubMenuWidget Interface

	/** Initialize the audio settings widget. */
	void Init();

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/**
	 * Event called when the master volume is changed.
	 *
	 * @param Volume	New master volume percentage (0-1). 
	 */
	UFUNCTION()
	void OnMasterVolumeChanged(const float Volume);

	/**
	 * Event called when the effects volume is changed.
	 *
	 * @param Volume	New effects volume percentage (0-1).  
	 */
	UFUNCTION()
	void OnEffectsVolumeChanged(const float Volume);
	
	/**
	 * Event called when the music volume is changed.
	 *
	 * @param Volume	New music volume percentage (0-1).  
	 */
	UFUNCTION()
	void OnMusicVolumeChanged(const float Volume);

	/**
	 * Event called when the interface volume is changed.
	 *
	 * @param Volume	New interface volume percentage (0-1).  
	 */
	UFUNCTION()
	void OnInterfaceVolumeChanged(const float Volume);

	/** Set the master volume (in the range [0, 1]). */
	void SetMasterVolume(const float Volume);

	/** Set the effects volume (in the range [0, 1]). */
	void SetEffectsVolume(const float Volume);

	/** Set the music volume (in the range [0, 1]). */
	void SetMusicVolume(const float Volume);

	/** Set the interface volume (in the range [0, 1]). */
	void SetInterfaceVolume(const float Volume);

	/** Save the currently set audio settings. */
	void SaveAudioSettings();

	/** Current audio settings. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UAudioUserSettings> AudioSettings;

	/** Sound class at the top of the hierarchy. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundClass> MasterSoundClass;

	/** Sound class for in-game sound effects. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundClass> EffectsSoundClass;

	/** Sound class for music. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundClass> MusicSoundClass;

	/** Sound class for interface sounds. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundClass> InterfaceSoundClass;

	/** Volume slider widget for the master volume. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVolumeSlider> MasterVolumeSlider;

	/** Volume slider widget for the effects volume. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVolumeSlider> EffectsVolumeSlider;

	/** Volume slider widget for the music volume. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVolumeSlider> MusicVolumeSlider;

	/** Volume slider widget for the interface volume. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVolumeSlider> InterfaceVolumeSlider;

};
