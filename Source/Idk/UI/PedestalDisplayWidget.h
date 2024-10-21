// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "PedestalDisplayWidget.generated.h"

class UBorder;
class UGenericDisplayIconWidget;
class USelectableWidgetBorder;
class UTextBlock;
class UWidget;
struct FPedestalDisplayInfo;

/** Widget that is used to display items on pedestals. */
UCLASS(Abstract)
class UPedestalDisplayWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the pedestal display widget.
	 * 
	 * @param DisplayInfo	Display information of the item to display. 
	 */
	void Init(const FPedestalDisplayInfo& DisplayInfo);

	/** Delegate called when the widget is hovered over. */
	FSimpleDelegate OnHoverBeginDelegate;

	/** Delegate called when the widget is no longer hovered over. */
	FSimpleDelegate OnHoverEndDelegate;

	/** Delegate called when the widget is selected by the player. */
	FSimpleDelegate OnSelectedDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Border around the widget that will be visible when hovered. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USelectableWidgetBorder> BorderGlow;

	/** Widget that displays information related to the price. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> PriceDisplay;

	/** Text that displays the price. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PriceText;

	/** Widget that displays the item's icon. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayIconWidget> Icon;

};
