// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/StatusEffectBarWidget.h"

#include "Idk/UI/StatusEffectWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/WrapBox.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult UStatusEffectBarWidget::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (StatusEffectWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Status effect widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

UStatusEffectWidget& UStatusEffectBarWidget::AddStatusEffect()
{
	check(!StatusEffectWidgetClass.IsNull());

	UStatusEffectWidget* StatusEffectWidget = CreateWidget<UStatusEffectWidget>(this, StatusEffectWidgetClass.LoadSynchronous());

	check(StatusEffectWidget);

	WrapBox->AddChildToWrapBox(StatusEffectWidget);

	return *StatusEffectWidget;

}

void UStatusEffectBarWidget::RemoveStatusEffect(UStatusEffectWidget& StatusEffect)
{
	WrapBox->RemoveChild(&StatusEffect);
}

void UStatusEffectBarWidget::ClearStatusEffects()
{
	WrapBox->ClearChildren();
}

void UStatusEffectBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Remove preview status effects
	ClearStatusEffects();
}
