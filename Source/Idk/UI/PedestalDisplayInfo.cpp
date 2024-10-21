// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/PedestalDisplayInfo.h"

#include <HAL/Platform.h>

FPedestalDisplayInfo::FPedestalDisplayInfo(const FGenericDisplayInfo& DisplayInfo, const int32 Price)
	: DisplayInfo(DisplayInfo), Price(Price)
{
}

const FGenericDisplayInfo& FPedestalDisplayInfo::GetDisplayInfo() const
{
	return DisplayInfo;
}

bool FPedestalDisplayInfo::HasPrice() const
{
	return Price >= 0;
}

int32 FPedestalDisplayInfo::GetPrice() const
{
	return Price;
}
