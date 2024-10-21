// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/VolumeSlider.h"

#include <AnalogSlider.h>
#include <Blueprint/UserWidget.h>
#include <Components/TextBlock.h>
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/UnrealMathUtility.h>

const FTextFormat UVolumeSlider::FormatText = FText::FromString(TEXT("{0}%"));

void UVolumeSlider::SetVolume(const float Volume)
{
	const float SliderValue = FMath::Floor(Volume * 100.f);

	Slider->SetValue(SliderValue);
	UpdateValueText(SliderValue);
}

float UVolumeSlider::GetVolume() const
{
	return FMath::Floor(Slider->GetValue()) / 100.f;
}

void UVolumeSlider::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Slider->OnValueChanged.AddDynamic(this, &UVolumeSlider::UpdateValueText);
	Slider->OnMouseCaptureEnd.AddDynamic(this, &UVolumeSlider::OnValueSet);
	Slider->OnControllerCaptureEnd.AddDynamic(this, &UVolumeSlider::OnValueSet);
}

void UVolumeSlider::UpdateValueText(const float Value)
{
	ValueText->SetText(FText::FormatOrdered(FormatText, FMath::FloorToInt(Value)));
}

void UVolumeSlider::OnValueSet()
{
	Slider->SetValue(FMath::Floor(Slider->GetValue()));

	OnValueSetDelegate.Broadcast(GetVolume());
}
