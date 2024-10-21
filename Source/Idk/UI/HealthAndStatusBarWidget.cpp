// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/HealthAndStatusBarWidget.h"

UHealthBarWidget& UHealthAndStatusBarWidget::GetHealthBarWidget() const
{
    return *HealthBarWidget;
}

UStatusEffectBarWidget& UHealthAndStatusBarWidget::GetStatusEffectBarWidget() const
{
    return *StatusEffectBarWidget;
}
