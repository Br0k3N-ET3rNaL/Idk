// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "HealthAndStatusBarWidget.generated.h"

class UHealthBarWidget;
class UStatusEffectBarWidget;

/** Widget that contains a health bar widget and a status effect bar widget. */
UCLASS(Abstract)
class UHealthAndStatusBarWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Get the health bar widget. */
	UE_NODISCARD UHealthBarWidget& GetHealthBarWidget() const;

	/** Get the status effect bar widget. */
	UE_NODISCARD UStatusEffectBarWidget& GetStatusEffectBarWidget() const;

private:
	/** Health bar widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBarWidget;

	/** Status effect bar widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusEffectBarWidget> StatusEffectBarWidget;
};
