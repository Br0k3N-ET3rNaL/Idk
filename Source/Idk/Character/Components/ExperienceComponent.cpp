// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/ExperienceComponent.h"

#include "Idk/UI/ExperienceWidget.h"
#include <Components/ActorComponent.h>
#include <Curves/CurveFloat.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/NumericLimits.h>
#include <Math/UnrealMathUtility.h>
#include <Math/UnrealPlatformMathSSE.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

UExperienceComponent::UExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
EDataValidationResult UExperienceComponent::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (ExperienceThresholds.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("UExperienceComponent: Experience thresholds curve is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();

	check(!ExperienceThresholds.IsNull())
	NextThreshold = FMath::FloorToInt32(ExperienceThresholds.LoadSynchronous()->GetFloatValue(Level + 1));
}

void UExperienceComponent::AddExperience(const int32 ExperienceToAdd)
{
	check(ExperienceWidget)

	if (ExperienceToAdd >= NextThreshold - Experience)
	{
		const uint8 OldLevel = Level;

		UCurveFloat* Thresholds = ExperienceThresholds.LoadSynchronous();
		int32 RemainingExperienceToAdd = ExperienceToAdd;

		// Handle possible integer overflow
		while (RemainingExperienceToAdd > MAX_int32 - Experience)
		{
			++Level;
			RemainingExperienceToAdd -= NextThreshold;
			NextThreshold = GetNextLevelThreshold(Level, *Thresholds);
		}

		int32 ExperienceRemaining = Experience + ExperienceToAdd;

		do
		{
			++Level;
			ExperienceRemaining -= NextThreshold;
			NextThreshold = GetNextLevelThreshold(Level, *Thresholds);
		} while (ExperienceRemaining >= NextThreshold);

		Experience = ExperienceRemaining;

		OnLevelUpDelegate.ExecuteIfBound(Level, Level - OldLevel, false);
	}
	else
	{
		Experience += ExperienceToAdd;
	}

	UpdateWidget();
}

uint8 UExperienceComponent::GetLevel() const
{
	return Level;
}

int32 UExperienceComponent::GetExperience() const
{
	return Experience;
}

void UExperienceComponent::RestoreLevelAndExperience(const uint8 InLevel, const int32 InExperience)
{
	const uint8 OldLevel = Level;
	Level = InLevel;
	Experience = InExperience;

	NextThreshold = FMath::FloorToInt32(ExperienceThresholds.LoadSynchronous()->GetFloatValue(Level + 1));

	UpdateWidget();

	OnLevelUpDelegate.ExecuteIfBound(Level, Level - OldLevel, true);
}

void UExperienceComponent::SetExperienceWidget(UExperienceWidget& InExperienceWidget)
{
	ExperienceWidget = &InExperienceWidget;
}

void UExperienceComponent::UpdateWidget()
{
	ExperienceWidget->SetLevelProgress((double)Experience / NextThreshold);
	ExperienceWidget->SetLevel(Level);
}

int32 UExperienceComponent::GetNextLevelThreshold(const uint8 CurrentLevel, const UCurveFloat& Thresholds)
{
	return FMath::FloorToInt32(Thresholds.GetFloatValue(CurrentLevel + 1));;
}
