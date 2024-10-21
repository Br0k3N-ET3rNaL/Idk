// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/AudioSettingsWidget.h"

#include "Idk/AudioUserSettings.h"
#include "Idk/UI/SettingsSubMenuWidget.h"
#include "Idk/UI/VolumeSlider.h"
#include <Delegates/Delegate.h>
#include <Sound/SoundClass.h>
#include <Templates/Casts.h>

void UAudioSettingsWidget::ApplySettings()
{
	// Apply current values for each volume slider
	AudioSettings->SetMasterVolume(MasterVolumeSlider->GetVolume());
	AudioSettings->SetEffectsVolume(EffectsVolumeSlider->GetVolume());
	AudioSettings->SetMusicVolume(MusicVolumeSlider->GetVolume());
	AudioSettings->SetInterfaceVolume(InterfaceVolumeSlider->GetVolume());

	SaveAudioSettings();
}

void UAudioSettingsWidget::ResetToDefaults()
{
	const UAudioUserSettings* DefaultAudioSettings = CastChecked<UAudioUserSettings>(UAudioUserSettings::StaticClass()->GetDefaultObject());

	SetMasterVolume(DefaultAudioSettings->GetMasterVolume());
	SetEffectsVolume(DefaultAudioSettings->GetEffectsVolume());
	SetMusicVolume(DefaultAudioSettings->GetMusicVolume());
	SetInterfaceVolume(DefaultAudioSettings->GetInterfaceVolume());

	ApplySettings();
}

void UAudioSettingsWidget::RevertToSavedValues()
{
	SetMasterVolume(AudioSettings->GetMasterVolume());
	SetEffectsVolume(AudioSettings->GetEffectsVolume());
	SetMusicVolume(AudioSettings->GetMusicVolume());
	SetInterfaceVolume(AudioSettings->GetInterfaceVolume());
}

void UAudioSettingsWidget::Init()
{
	AudioSettings = UAudioUserSettings::LoadOrCreateAudioSettings();

	RevertToSavedValues();
}

void UAudioSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MasterVolumeSlider->OnValueSetDelegate.AddDynamic(this, &UAudioSettingsWidget::OnMasterVolumeChanged);
	EffectsVolumeSlider->OnValueSetDelegate.AddDynamic(this, &UAudioSettingsWidget::OnEffectsVolumeChanged);
	MusicVolumeSlider->OnValueSetDelegate.AddDynamic(this, &UAudioSettingsWidget::OnMusicVolumeChanged);
	InterfaceVolumeSlider->OnValueSetDelegate.AddDynamic(this, &UAudioSettingsWidget::OnInterfaceVolumeChanged);
}

void UAudioSettingsWidget::OnMasterVolumeChanged(const float Volume)
{
	MasterSoundClass->Properties.Volume = Volume;
}

void UAudioSettingsWidget::OnEffectsVolumeChanged(const float Volume)
{
	EffectsSoundClass->Properties.Volume = Volume;
}

void UAudioSettingsWidget::OnMusicVolumeChanged(const float Volume)
{
	MusicSoundClass->Properties.Volume = Volume;
}

void UAudioSettingsWidget::OnInterfaceVolumeChanged(const float Volume)
{
	InterfaceSoundClass->Properties.Volume = Volume;
}

void UAudioSettingsWidget::SetMasterVolume(const float Volume)
{
	MasterSoundClass->Properties.Volume = Volume;
	MasterVolumeSlider->SetVolume(Volume);
}

void UAudioSettingsWidget::SetEffectsVolume(const float Volume)
{
	EffectsSoundClass->Properties.Volume = Volume;
	EffectsVolumeSlider->SetVolume(Volume);
}

void UAudioSettingsWidget::SetMusicVolume(const float Volume)
{
	MusicSoundClass->Properties.Volume = Volume;
	MusicVolumeSlider->SetVolume(Volume);
}

void UAudioSettingsWidget::SetInterfaceVolume(const float Volume)
{
	InterfaceSoundClass->Properties.Volume = Volume;
	InterfaceVolumeSlider->SetVolume(Volume);
}

void UAudioSettingsWidget::SaveAudioSettings()
{
	AudioSettings->SaveAudioSettings();
}
