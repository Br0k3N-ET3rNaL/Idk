// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/TooltipWidget.h"

#include <Components/RichTextBlock.h>
#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>

const FTextFormat UTooltipWidget::DisabledFormatText = FText::FromString(TEXT("<Disabled>{0}</>"));

void UTooltipWidget::Init(const FText& InTitle, const FText& InDescription)
{
	Title->SetText(InTitle);
	Description->SetText(InDescription);
}

void UTooltipWidget::UpdateDescription(const FText& UpdatedDescription)
{
	Description->SetText(UpdatedDescription);
}

void UTooltipWidget::DisableDescription()
{
	Description->SetText(FText::FormatOrdered(DisabledFormatText, Description->GetText()));
}
