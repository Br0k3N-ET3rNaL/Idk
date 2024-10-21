// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GenericDisplayButtonWidget.h"

#include "Idk/UI/GenericDisplayWidget.h"
#include <Components/Button.h>
#include <Delegates/Delegate.h>

void UGenericDisplayButtonWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	GenericDisplayWidget->Init(DisplayInfo);
}

void UGenericDisplayButtonWidget::NativeOnInitialized()
{
	Button->OnClicked.AddDynamic(this, &UGenericDisplayButtonWidget::OnButtonClicked);
	Button->OnHovered.AddDynamic(GenericDisplayWidget, &UGenericDisplayWidget::OnHoverBegin);
	Button->OnUnhovered.AddDynamic(GenericDisplayWidget, &UGenericDisplayWidget::OnHoverEnd);
}

void UGenericDisplayButtonWidget::OnButtonClicked()
{
	OnButtonClickedDelegate.ExecuteIfBound();
}
