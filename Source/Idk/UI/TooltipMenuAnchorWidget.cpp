// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/TooltipMenuAnchorWidget.h"

#include "Idk/UI/TooltipWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/MenuAnchor.h>
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult UTooltipMenuAnchorWidget::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (CustomTooltipClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Custom tooltip class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UTooltipMenuAnchorWidget::Init(const FText& Title, const FText& Description)
{
	check(!CustomTooltipClass.IsNull());

	if (!CustomTooltipWidget)
	{
		CustomTooltipWidget = CreateWidget<UTooltipWidget>(this, CustomTooltipClass.LoadSynchronous());
		MenuAnchor->OnGetUserMenuContentEvent.BindDynamic(this, &UTooltipMenuAnchorWidget::GetTooltipWidgetInternal);
	}

	CustomTooltipWidget->Init(Title, Description);
}

void UTooltipMenuAnchorWidget::ShowTooltip()
{
	MenuAnchor->Open(false);
}

void UTooltipMenuAnchorWidget::HideTooltip()
{
	MenuAnchor->Close();
}

void UTooltipMenuAnchorWidget::DisableDescription()
{
	CustomTooltipWidget->DisableDescription();
}

UTooltipWidget& UTooltipMenuAnchorWidget::GetTooltipWidget()
{
	return *CustomTooltipWidget;
}

UUserWidget* UTooltipMenuAnchorWidget::GetTooltipWidgetInternal()
{
	return CustomTooltipWidget;
}
