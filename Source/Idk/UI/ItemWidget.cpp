// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/ItemWidget.h"

#include "Idk/UI/GenericDisplayWidget.h"
#include <Components/SlateWrapperTypes.h>
#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>

void UItemWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	DisplayWidget->Init(DisplayInfo);

	StackText->SetVisibility(ESlateVisibility::Hidden);
}

void UItemWidget::SetItemStacks(const uint8 Stacks)
{
	if (Stacks == 1)
	{
		StackText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		StackText->SetText(FText::AsNumber(Stacks));

		StackText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UItemWidget::DisableItem()
{
	DisplayWidget->DisableDisplay();
}
