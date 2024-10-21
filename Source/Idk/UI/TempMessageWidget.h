// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "TempMessageWidget.generated.h"

class FText;
class UTextBlock;
class UWidgetAnimation;

/** Widget used to display a message to the player that fades over time. */
UCLASS(Abstract)
class UTempMessageWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Show a message. 
	 * 
	 * @param InMessage	Message to display. 
	 * @param FadeDelay	How long to wait before fading out the message.
	 */
	void ShowMessage(const FText& InMessage, const float FadeDelay = 0.f);

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Text used to display the message. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Message;

	/** Widget animation used to fade out the widget. */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Fade;

};
