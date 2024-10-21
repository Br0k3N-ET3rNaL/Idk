// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GenericDisplayWidget.generated.h"

class UGenericDisplayIconWidget;
class UTooltipMenuAnchorWidget;
class UWidget;
struct FGenericDisplayInfo;
struct FGeometry;
struct FPointerEvent;

/** Widget that can display anything with an icon, name, and description. */
UCLASS(Abstract)
class UGenericDisplayWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

	/**
	 * Initialize the display widget.

	 * @param DisplayInfo	Information that determines what to display. @see FGenericDisplayInfo 
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

	/** Gray out the display and hide tooltip. */
	void DisableDisplay();

	/** Event called when the mouse hovers over the display. */
	UFUNCTION()
	void OnHoverBegin();

	/** Event called when the mouse is no longer hovered over the display. */
	UFUNCTION()
	void OnHoverEnd();

private:
	/** Icon for the display. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayIconWidget> Icon;

	/** Tooltip for the display. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTooltipMenuAnchorWidget> TooltipMenuAnchor;

	/** Overlay that is shown when the display is disabled. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> DisabledOverlay;

	/** Whether to display the tooltip when hovered over. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	bool bShowTooltip = false;
};
