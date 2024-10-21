// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/PedestalDisplayWidget.h"

#include "Idk/UI/GenericDisplayIconWidget.h"
#include "Idk/UI/PedestalDisplayInfo.h"
#include "Idk/UI/SelectableWidgetBorder.h"
#include <Components/SlateWrapperTypes.h>
#include <Components/TextBlock.h>
#include <Internationalization/Text.h>

void UPedestalDisplayWidget::Init(const FPedestalDisplayInfo& DisplayInfo)
{
	Icon->Init(DisplayInfo.GetDisplayInfo());

	if (DisplayInfo.HasPrice())
	{
		PriceText->SetText(FText::AsNumber(DisplayInfo.GetPrice()));
		PriceDisplay->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		PriceDisplay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPedestalDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BorderGlow->OnSelectedDelegate.BindLambda([&]()
		{
			OnSelectedDelegate.ExecuteIfBound();
		});

	BorderGlow->OnHoverBeginDelegate.BindLambda([&]()
		{
			OnHoverBeginDelegate.ExecuteIfBound();
		});

	BorderGlow->OnHoverEndDelegate.BindLambda([&]()
		{
			OnHoverEndDelegate.ExecuteIfBound();
		});
}
