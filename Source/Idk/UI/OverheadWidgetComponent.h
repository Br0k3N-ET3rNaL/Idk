// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/WidgetComponent.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>

#include "OverheadWidgetComponent.generated.h"

struct FPropertyChangedEvent;

/** Component used to display a widget above a character's head. */
UCLASS()
class UOverheadWidgetComponent final : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UOverheadWidgetComponent();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

private:

	void UpdateDrawSize();

	/** Size of the created widget. */
	UPROPERTY(EditDefaultsOnly)
	FVector2D BaseDrawSize = { 100.0, 20.0 };

	/** How much to upscale the created widget. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.0))
	double UpscaleFactor = 5.0;
};
