// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GoldWidget.h"

#include <Components/TextBlock.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>

void UGoldWidget::SetGold(const int32 Gold)
{
	GoldText->SetText(FText::AsNumber(Gold, &FNumberFormattingOptions::DefaultNoGrouping()));
}
