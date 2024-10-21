// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GenericDisplayIconWidget.h"

#include "Idk/UI/GenericDisplayInfo.h"
#include <Components/Border.h>
#include <Components/Image.h>
#include <Layout/Margin.h>
#include <Math/Color.h>
#include <PaperSprite.h>

void UGenericDisplayIconWidget::Init(const FGenericDisplayInfo& DisplayInfo)
{
	if (UPaperSprite* IconImage = DisplayInfo.GetIcon())
	{
		SetIcon(*IconImage);
	}

	if (DisplayInfo.IsBorderColorSet())
	{
		SetBorderColor(DisplayInfo.GetBorderColor());
	}
}

void UGenericDisplayIconWidget::SetBorderColor(const FLinearColor& BorderColor)
{
	Border->SetBrushColor(BorderColor);
	Border->SetPadding(FMargin(BorderWidth));
}

void UGenericDisplayIconWidget::SetIcon(UPaperSprite& Icon)
{
	Image->SetBrushFromAtlasInterface(&Icon);
	Image->SetColorAndOpacity(FLinearColor::White);
}
