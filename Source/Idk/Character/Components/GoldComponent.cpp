// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/Components/GoldComponent.h"

#include "Idk/UI/GoldWidget.h"
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>

UGoldComponent::UGoldComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGoldComponent::SetGold(const int32 InGold)
{
	check(InGold >= 0);

	Gold = InGold;

	UpdateWidget();
}

void UGoldComponent::AddGold(const int32 GoldToAdd)
{
	Gold = (GoldToAdd > MaxGold - Gold) ? MaxGold : Gold + GoldToAdd;

	UpdateWidget();
}

void UGoldComponent::RemoveGold(const int32 GoldToLose)
{
	Gold = (GoldToLose > Gold) ? 0 : Gold - GoldToLose;

	UpdateWidget();
}

int32 UGoldComponent::GetGold() const
{
	return Gold;
}

void UGoldComponent::SetGoldWidget(UGoldWidget& InGoldWidget)
{
	GoldWidget = &InGoldWidget;

	UpdateWidget();
}

void UGoldComponent::UpdateWidget()
{
	check(GoldWidget)

	GoldWidget->SetGold(Gold);
}
