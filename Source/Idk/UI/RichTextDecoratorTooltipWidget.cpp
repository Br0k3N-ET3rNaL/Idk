// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/RichTextDecoratorTooltipWidget.h"

#include "Idk/UI/TooltipMenuAnchorWidget.h"
#include <Components/SlateWrapperTypes.h>
#include <Components/TextBlock.h>
#include <Input/Events.h>
#include <Internationalization/Text.h>
#include <Layout/Geometry.h>
#include <Styling/SlateTypes.h>

void URichTextDecoratorTooltipWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	TooltipMenuAnchor->ShowTooltip();
}

void URichTextDecoratorTooltipWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	TooltipMenuAnchor->HideTooltip();
}

void URichTextDecoratorTooltipWidget::Init(const FText& Title, const FText& Description, const FTextBlockStyle& TextBlockStyle)
{
	TextBlock->SetText(Title.ToLower());
	TextBlock->SetVisibility(ESlateVisibility::Visible);

	// Set the text style
	TextBlock->SetColorAndOpacity(TextBlockStyle.ColorAndOpacity);
	TextBlock->SetFont(TextBlockStyle.Font);
	TextBlock->SetShadowColorAndOpacity(TextBlockStyle.ShadowColorAndOpacity);
	TextBlock->SetShadowOffset(TextBlockStyle.ShadowOffset);
	TextBlock->SetStrikeBrush(TextBlockStyle.StrikeBrush);

	TooltipMenuAnchor->Init(Title, Description);
}
