// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GoldWidget.generated.h"

class UTextBlock;

/** Widget that displays the player's current gold. */
UCLASS(Abstract)
class UGoldWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Set the current amount of gold. */
	void SetGold(const int32 Gold);
	
private:
	/** Text that displays the current amount of gold. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;
};
