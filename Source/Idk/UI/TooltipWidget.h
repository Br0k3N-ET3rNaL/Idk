// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "TooltipWidget.generated.h"

class URichTextBlock;
class UTextBlock;

/** Widget used to display a tooltip. */
UCLASS(Abstract)
class UTooltipWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the tooltip widget.
	 * 
	 * @param InTitle		Title to display.
	 * @param InDescription	Description to display. 
	 */
	void Init(const FText& InTitle, const FText& InDescription);

	/** Update the tooltip's description. */
	void UpdateDescription(const FText& UpdatedDescription);

	/** Gray out the description. */
	void DisableDescription();

private:
	/** Text used to display the title for the tooltip. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Title;

	/** Text used to display the description for the tooltip. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> Description;

	/** Format used to gray out text. */
	static const FTextFormat DisabledFormatText;
};
