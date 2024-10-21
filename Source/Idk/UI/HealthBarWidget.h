// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "HealthBarWidget.generated.h"

class UProgressBar;

/** Widget that displays a character's current health percentage. */
UCLASS(Abstract)
class UHealthBarWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Set the current health percentage (in the range [0.0,1.0]). */
	void SetHealthPercent(const double HealthPercent = 1.0);

private:
	/** Progress bar representing the current health percentage. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

};
