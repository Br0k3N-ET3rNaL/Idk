// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "ExperienceWidget.generated.h"

class UProgressBar;
class UTextBlock;

/** Widget that displays the player's level and progress to the next level. */
UCLASS(Abstract)
class UExperienceWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Set the progress to the next level (in the range [0,1]). */
	void SetLevelProgress(const double LevelProgress);

	/** Set the current level. */
	void SetLevel(const uint8 Level);

private:
	/** Progress bar that represents the progress to the next level. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> LevelProgressBar;

	/** Text that displays the current level. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;
};
