// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/DelegateCombinations.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "ConfirmationWidget.generated.h"

class FText;
class USelectableWidgetBorder;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FOnOptionSelectedDelegate, bool);

/** Widget that shows a message to the player and allows them to either confirm or deny. */
UCLASS(Abstract)
class UConfirmationWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Set the message to display. */
	void SetMessage(const FText& Message);

	/**
	 * Delegate called when an option is selected.
	 *
	 * @param bConfirmed	Whether confirm or deny was selected.
	 */
	FOnOptionSelectedDelegate OnOptionSelectedDelegate;

private:
	/** Text used to display the message. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock;

	/** Button to confirm. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USelectableWidgetBorder> ConfirmButton;

	/** Button to deny. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USelectableWidgetBorder> DenyButton;
};
