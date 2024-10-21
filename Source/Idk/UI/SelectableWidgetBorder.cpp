// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/SelectableWidgetBorder.h"

#include <Components/Image.h>
#include <Components/SlateWrapperTypes.h>
#include <Input/Events.h>
#include <Input/Reply.h>
#include <Layout/Geometry.h>

void USelectableWidgetBorder::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BorderGlow->SetVisibility(ESlateVisibility::Hidden);
}

FReply USelectableWidgetBorder::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnSelectedDelegate.ExecuteIfBound();

	return FReply::Handled();
}

void USelectableWidgetBorder::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	BorderGlow->SetVisibility(ESlateVisibility::Visible);
	OnHoverBeginDelegate.ExecuteIfBound();
}

void USelectableWidgetBorder::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	BorderGlow->SetVisibility(ESlateVisibility::Hidden);
	OnHoverEndDelegate.ExecuteIfBound();
}
