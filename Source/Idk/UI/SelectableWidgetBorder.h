// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <Input/Reply.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "SelectableWidgetBorder.generated.h"

class UImage;
struct FGeometry;
struct FPointerEvent;

/** Widget that displays a colored border around a child widget when hovered over. */
UCLASS(Abstract)
class USelectableWidgetBorder final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

	/** Delegate called when the widget is hovered over. */
	FSimpleDelegate OnHoverBeginDelegate;

	/** Delegate called when the widget is no longer hovered over. */
	FSimpleDelegate OnHoverEndDelegate;

	/** Delegate called when the widget is clicked on. */
	FSimpleDelegate OnSelectedDelegate;

private:
	/** Colored border around child widget.  */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BorderGlow;

};
