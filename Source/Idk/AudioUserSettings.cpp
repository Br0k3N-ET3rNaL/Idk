// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AudioUserSettings.h"

#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Kismet/GameplayStatics.h>
#include <Templates/Casts.h>

const FString UAudioUserSettings::SlotName = TEXT("AudioUserSettings");
const int32 UAudioUserSettings::UserIndex = 0;

void UAudioUserSettings::SetMasterVolume(const float InMasterVolume)
{
	MasterVolume = InMasterVolume;
}

void UAudioUserSettings::SetEffectsVolume(const float InEffectsVolume)
{
	EffectsVolume = InEffectsVolume;
}

void UAudioUserSettings::SetMusicVolume(const float InMusicVolume)
{
	MusicVolume = InMusicVolume;
}

void UAudioUserSettings::SetInterfaceVolume(const float InInterfaceVolume)
{
	InterfaceVolume = InInterfaceVolume;
}

float UAudioUserSettings::GetMasterVolume() const
{
	return MasterVolume;
}

float UAudioUserSettings::GetEffectsVolume() const
{
	return EffectsVolume;
}

float UAudioUserSettings::GetMusicVolume() const
{
	return MusicVolume;
}

float UAudioUserSettings::GetInterfaceVolume() const
{
	return InterfaceVolume;
}

void UAudioUserSettings::SaveAudioSettings()
{
	UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
}

UAudioUserSettings& UAudioUserSettings::LoadOrCreateAudioSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return *CastChecked<UAudioUserSettings>(
			UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	}
	else
	{
		return *CastChecked<UAudioUserSettings>(
			UGameplayStatics::CreateSaveGameObject(StaticClass()));
	}
}
