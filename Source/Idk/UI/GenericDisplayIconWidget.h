// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GenericDisplayIconWidget.generated.h"

class UBorder;
class UImage;
class UPaperSprite;
struct FGenericDisplayInfo;
struct FLinearColor;

/** Widget that displays an icon with an optional colored border. */
UCLASS(Abstract)
class UGenericDisplayIconWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the display icon.
	 * 
	 * @param DisplayInfo	Information that determines what to display. @see FGenericDisplayInfo  
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

private:
	/** Set the color of the border. */
	void SetBorderColor(const FLinearColor& BorderColor);

	/** Set the icon to display. */
	void SetIcon(UPaperSprite& Icon);

	/** Colored border around the icon. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border;

	/** The icon. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image;

	/** Width of the border. */
	UPROPERTY(EditDefaultsOnly)
	double BorderWidth = 1.0;

};
