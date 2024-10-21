// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/StatusEffectWidget.h"

#include "Idk/UI/GenericDisplayWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Misc/AssertionMacros.h>
#include <UObject/NameTypes.h>

const FName UStatusEffectWidget::FillPercent = TEXT("FillPercent");

void UStatusEffectWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	DisplayWidget->Init(DisplayInfo);
}

void UStatusEffectWidget::UpdateTimeRemaining(const double DurationRemainingPercent)
{
	check(DurationRemainingPercent >= 0.0 && DurationRemainingPercent <= 1.0);

	DurationRemainingOverlay->GetDynamicMaterial()->SetScalarParameterValue(FillPercent, 1.0 - DurationRemainingPercent);
}

void UStatusEffectWidget::UpdateStacks(const uint8 Stacks)
{
	StacksCounter->SetText(FText::AsNumber(Stacks));
}

void UStatusEffectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RadialProgressMaterialInstance = UMaterialInstanceDynamic::Create(RadialProgressMaterial.LoadSynchronous(), this);
	DurationRemainingOverlay->SetBrushFromMaterial(RadialProgressMaterialInstance);
}
