// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/HealthBarWidget.h"

#include <Components/ProgressBar.h>

void UHealthBarWidget::SetHealthPercent(const double HealthPercent)
{
	ProgressBar->SetPercent(HealthPercent);
}
