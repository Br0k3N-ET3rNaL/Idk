// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/ExperienceWidget.h"

#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>

void UExperienceWidget::SetLevelProgress(const double LevelProgress)
{
	check(LevelProgress >= 0.0 && LevelProgress <= 1.0)

	LevelProgressBar->SetPercent(LevelProgress);
}

void UExperienceWidget::SetLevel(const uint8 Level)
{
	LevelText->SetText(FText::AsNumber(Level));
}
