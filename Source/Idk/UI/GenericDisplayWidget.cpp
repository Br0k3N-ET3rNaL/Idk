// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GenericDisplayWidget.h"

#include "Idk/UI/GenericDisplayIconWidget.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include "Idk/UI/TooltipMenuAnchorWidget.h"
#include "Idk/UI/TooltipWidget.h"
#include <Components/SlateWrapperTypes.h>
#include <Input/Events.h>
#include <Layout/Geometry.h>
#include <Templates/Casts.h>

void UGenericDisplayWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnHoverBegin();
}

void UGenericDisplayWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnHoverEnd();
}

void UGenericDisplayWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	TooltipMenuAnchor->Init(DisplayInfo.GetDisplayName(), DisplayInfo.GetDescription());

	Icon->Init(DisplayInfo);

	bShowTooltip = true;

	UTooltipWidget& Tooltip = TooltipMenuAnchor->GetTooltipWidget();

	DisplayInfo.UpdateDescriptionDelegate.AddUObject(&Tooltip, &UTooltipWidget::UpdateDescription);
}

void UGenericDisplayWidget::DisableDisplay()
{
	DisabledOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	TooltipMenuAnchor->DisableDescription();
}

void UGenericDisplayWidget::OnHoverBegin()
{
	if (bShowTooltip)
	{
		TooltipMenuAnchor->ShowTooltip();
	}
}

void UGenericDisplayWidget::OnHoverEnd()
{
	if (bShowTooltip)
	{
		TooltipMenuAnchor->HideTooltip();
	}
}
