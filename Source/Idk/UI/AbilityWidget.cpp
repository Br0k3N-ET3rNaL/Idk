// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/AbilityWidget.h"

#include "Idk/UI/GenericDisplayWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <Internationalization/Text.h>

void UAbilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NumberFormatting = FNumberFormattingOptions::DefaultNoGrouping();
	NumberFormatting.SetMaximumFractionalDigits(0);
	NumberFormatting.SetRoundingMode(ERoundingMode::ToPositiveInfinity);
}

void UAbilityWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	DisplayWidget->Init(DisplayInfo);
}

void UAbilityWidget::UpdateCooldown(const double SecondsRemaining, const double PercentRemaining)
{
	const FText SecondsRemainingText = FText::AsNumber(SecondsRemaining, &NumberFormatting);

	CooldownRemaining->SetText(SecondsRemainingText);
	CooldownPercent->SetPercent(PercentRemaining);
}

void UAbilityWidget::DisableAbility()
{
	// Effectively grays out the ability
	CooldownPercent->SetPercent(1.f);
}

void UAbilityWidget::EndCooldown()
{
	CooldownRemaining->SetText(FText());
	CooldownPercent->SetPercent(0.f);

	PlayAnimation(CooldownDoneAnimation);
}
