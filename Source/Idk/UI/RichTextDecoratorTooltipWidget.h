// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "RichTextDecoratorTooltipWidget.generated.h"

class FText;
class UTextBlock;
class UTooltipMenuAnchorWidget;
struct FGeometry;
struct FPointerEvent;
struct FTextBlockStyle;

/** Widget embedded in tooltips to allow for nested tooltips. */
UCLASS(Abstract)
class URichTextDecoratorTooltipWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

	/**
	 * Initialize the tooltip widget.
	 * 
	 * @param Title				Title to use for the tooltip. 
	 * @param Description		Description to use for the tooltip.
	 * @param TextBlockStyle	Text style to use for the embedded title. 
	 */
	void Init(const FText& Title, const FText& Description, const FTextBlockStyle& TextBlockStyle);

private:
	/** Text that displays the embedded title. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock;

	/** Widget that displays the tooltip. @see UTooltipMenuAnchorWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTooltipMenuAnchorWidget> TooltipMenuAnchor;
	
};
