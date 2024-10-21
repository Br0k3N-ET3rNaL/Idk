// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Components/Slider.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "VolumeSlider.generated.h"

class UAnalogSlider;
class UTextBlock;

/** Widget that displays a slider used to adjust volume. */
UCLASS(Abstract)
class UVolumeSlider final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Set the volume percentage (in the range [0.0,1.0]) */
	void SetVolume(const float Volume);

	/** Get the current volume percentage (in the range [0.0,1.0]). */
	UE_NODISCARD float GetVolume() const;

	/**
	 * Delegate called after the slider's value changes.
	 * 
	 * @param Value	Current volume percentage (in the range [0.0,1.0]).	
	 */
	FOnFloatValueChangedEvent OnValueSetDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Update the text displaying the current volume percentage. */
	UFUNCTION()
	void UpdateValueText(const float Value);

	/** Event called after the slider's value changes. */
	UFUNCTION()
	void OnValueSet();

	/** Slider representing the volume percentage. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAnalogSlider> Slider;

	/** Text displaying the current volume percentage (in the range [0,100]). */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;

	/** Text format used to add '%' to the end of the text. */
	static const FTextFormat FormatText;
};
