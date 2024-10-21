// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "AbilityChoiceWidget.generated.h"

class UGenericDisplayButtonWidget;
class UInputAction;
class UUniformGridPanel;
struct FAbilityInfo;

DECLARE_DELEGATE_OneParam(FAbilityChosenDelegate, int32);

/** Widget that allows the player to select an ability. */
UCLASS(Abstract)
class UAbilityChoiceWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Set the abilities that the player can choose from. */
	void SetChoices(const TArray<const FAbilityInfo*>& AbilityChoices);

	/**
	 * Delegate called when an ability is chosen.
	 * 
	 * @param Index	Index of the chosen ability.
	 */
	FAbilityChosenDelegate AbilityChosenDelegate;

private:
	/** Grid panel containing the ability choices. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> AbilityGrid;

	/** Class of the widget used to display individual abilities. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<UGenericDisplayButtonWidget> DisplayButtonWidgetClass;

};
