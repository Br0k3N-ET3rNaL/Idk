// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GenericDisplayButtonWidget.generated.h"

class UButton;
class UGenericDisplayWidget;
struct FGenericDisplayInfo;

/** Clickable version of UGenericDisplayWidget. */
UCLASS(Abstract)
class UGenericDisplayButtonWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the display button.
	 * 
	 * @param DisplayInfo	Information that determines what to display. @see FGenericDisplayInfo   
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

	/** Delegate called when the button is clicked. */
	FSimpleDelegate OnButtonClickedDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Event called when the button is clicked. */
	UFUNCTION()
	void OnButtonClicked();

	/** The button. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;

	/** Widget that displays an icon with a tooltip. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> GenericDisplayWidget;
};
