// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "ItemWidget.generated.h"

class UGenericDisplayWidget;
class UTextBlock;
struct FGenericDisplayInfo;

/** Widget used to display an item in the player's inventory. */
UCLASS(Abstract)
class UItemWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the item widget.
	 * 
	 * @param DisplayInfo	Information used to display the item. @see FGenericDisplayInfo 
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

	/** Set the number of stacks for the item. */
	void SetItemStacks(const uint8 Stacks);

	/** Gray out the item. */
	void DisableItem();

private:
	/** Widget that displays the item's icon and tooltip. @see UGenericDisplayWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> DisplayWidget;

	/** Text for the current number of stacks. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackText;
};
