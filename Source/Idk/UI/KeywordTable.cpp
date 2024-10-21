// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/KeywordTable.h"

#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/Color.h>

FKeywordInfo::FKeywordInfo(const FString& Description, const FLinearColor& TextColor)
	: FKeywordInfo(FText::FromString(Description), TextColor)
{
}

FKeywordInfo::FKeywordInfo(const FText& Description, const FLinearColor& TextColor)
	: Description(Description), TextColor(TextColor)
{
}

const FText& FKeywordInfo::GetDescription() const
{
	return Description;
}

const FLinearColor& FKeywordInfo::GetTextColor() const
{
	return TextColor;
}

UKeywordTable::UKeywordTable()
{
	KeywordTable.Add(TEXT("Test"), FKeywordInfo(TEXT("Custom keyword description."), FLinearColor::Red));
}
