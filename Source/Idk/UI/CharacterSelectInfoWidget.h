// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "CharacterSelectInfoWidget.generated.h"

class FText;
class UTextBlock;

/** Widget that displays information about a playable class during character selection. */
UCLASS(Abstract)
class UCharacterSelectInfoWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the character select info widget.
	 * 
	 * @param InName		Name of the class. 
	 * @param InDescription Description of the class.
	 */
	void Init(const FText& InName, const FText& InDescription);

private:
	/** Text that displays the class's name. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;

	/** Text that displays the class's description. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Description;
};
