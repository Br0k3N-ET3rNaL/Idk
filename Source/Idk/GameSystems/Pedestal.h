// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "Pedestal.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UPedestalDisplayWidget;
class UTooltipWidget;
class UWidgetComponent;
struct FPedestalDisplayInfo;

/** A pedestal that can be selected and used to display items to the player. */
UCLASS(Abstract, Blueprintable)
class APedestal final : public AActor
{
	GENERATED_BODY()
	
public:	
	APedestal();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the pedestal.
	 * 
	 * @param DisplayInfo	Display information of the item to display. 
	 */
	void Init(const FPedestalDisplayInfo& DisplayInfo);

	/** Delegate called when the pedestal is selected by the player. */
	FSimpleDelegate PedestalSelectedDelegate;

private:
	/** Event called when the pedestal is hovered over by the player. */
	void OnHover();

	/** Event called when the pedestal is no longer hovered over by the player. */
	void OnHoverEnd();

	/** Width & height to draw the widget at. */
	UPROPERTY(EditDefaultsOnly)
	double PedestalDisplayDrawSize = 50.0;

	/** How much to upscale the created widget. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.0))
	double PedestalDisplayUpscaleFactor = 5.0;

	/** Blueprint class of the widget to display. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UPedestalDisplayWidget> PedestalDisplayWidgetClass;

	/** Blueprint class of the widget used to display a tooltip. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UTooltipWidget> TooltipWidgetClass;

	/** Widget to display. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PedestalDisplayWidgetComp;

	/** Widget that shows a tooltip. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> TooltipWidgetComp;

};
