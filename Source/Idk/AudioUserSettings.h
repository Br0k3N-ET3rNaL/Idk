// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/UnrealString.h>
#include <GameFramework/SaveGame.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "AudioUserSettings.generated.h"

/** Contains audio related settings. */
UCLASS()
class UAudioUserSettings final : public USaveGame
{
	GENERATED_BODY()
	
public:
	/** Set the master volume percentage (in the range [0.0,1.0]). */
	void SetMasterVolume(const float InMasterVolume);

	/** Set the effects volume percentage (in the range [0.0,1.0]). */
	void SetEffectsVolume(const float InEffectsVolume);

	/** Set the music volume percentage (in the range [0.0,1.0]). */
	void SetMusicVolume(const float InMusicVolume);

	/** Set the interface volume percentage (in the range [0.0,1.0]). */
	void SetInterfaceVolume(const float InInterfaceVolume);

	/** Get the master volume percentage (in the range [0.0,1.0]). */
	UE_NODISCARD float GetMasterVolume() const;

	/** Get the effects volume percentage (in the range [0.0,1.0]). */
	UE_NODISCARD float GetEffectsVolume() const;

	/** Get the music volume percentage (in the range [0.0,1.0]). */
	UE_NODISCARD float GetMusicVolume() const;

	/** Get the interface volume percentage (in the range [0.0,1.0]). */
	UE_NODISCARD float GetInterfaceVolume() const;

	/** Save the current settings. */
	void SaveAudioSettings();

	/** Load existing audio settings or create them if they don't already exist. */
	UE_NODISCARD static UAudioUserSettings& LoadOrCreateAudioSettings();

private:
	/** Master volume percentage (in the range [0.0,1.0]). */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	float MasterVolume = 1.f;

	/** Effects volume percentage (in the range [0.0,1.0]). */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	float EffectsVolume = 1.f;

	/** Music volume percentage (in the range [0.0,1.0]). */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	float MusicVolume = 1.f;

	/** Interface volume percentage (in the range [0.0,1.0]). */
	UPROPERTY(VisibleAnywhere, Category = "Idk")
	float InterfaceVolume = 1.f;

	/** Name of the slot to save audio settings to. */
	static const FString SlotName;

	/** Index of the user used to save audio settings. */
	static const int32 UserIndex;
};
