// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/ConfirmationWidget.h"

#include "Idk/UI/SelectableWidgetBorder.h"
#include <Components/TextBlock.h>
#include <Internationalization/Text.h>

void UConfirmationWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConfirmButton->OnSelectedDelegate.BindLambda([&]()
		{
			OnOptionSelectedDelegate.ExecuteIfBound(true);
		});

	DenyButton->OnSelectedDelegate.BindLambda([&]()
		{
			OnOptionSelectedDelegate.ExecuteIfBound(false);
		});
}

void UConfirmationWidget::SetMessage(const FText& Message)
{
	TextBlock->SetText(Message);
}
