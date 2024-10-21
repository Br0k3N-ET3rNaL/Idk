// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilitySwapWidget.generated.h"

class UCommonButton;
class UGenericDisplayWidget;
struct FAbilityInfo;

DECLARE_DELEGATE_OneParam(FSetAbilityDelegate, const int32);

/** Widget that allows the player to swap a chosen ability for existing ones. */
UCLASS(Abstract)
class UAbilitySwapWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Set the abilities to display.

	 * @param NewAbility	Newly chosen ability. 
	 * @param Ability1		Ability in the player's first ability slot.
	 * @param Ability2		Ability in the player's second ability slot.
	 */
	void SetAbilities(const FAbilityInfo& NewAbility, const FAbilityInfo* Ability1, const FAbilityInfo* Ability2);

	/**
	 * Delegate called when the new ability is swapped for an existing one.
	 * 
	 * @param	AbilityIndex Index of the player's ability slot to set the ability for.
	 */
	FSetAbilityDelegate SetAbilityDelegate;

	/** Delegate called when the chosen ability is discarded. */
	FSimpleDelegate DiscardAbilityDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Event called when the discard button is clicked. */
	void OnDiscardButtonClicked() const;

	/** Event called when the button for swapping with the first ability is clicked. */
	void OnAbility1ButtonClicked() const;

	/** Event called when the button for swapping with the second ability is clicked. */
	void OnAbility2ButtonClicked() const;

	/** Button that discards the newly chosen ability. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> DiscardButton;

	/** Button that swaps the chosen ability with the player's first ability slot. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> Ability1Button;

	/** Button that swaps the chosen ability with the player's second ability slot. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> Ability2Button;

	/** Displays the newly chosen ability. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> NewAbilityDisplay;

	/** Displays the ability in the player's first ability slot. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> Ability1Display;

	/** Displays the ability in the player's second ability slot.  */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> Ability2Display;

	/** Text for the button for setting an unset ability. */
	UPROPERTY(EditDefaultsOnly)
	FText SetAbilityButtonText;

	/** Text for the button for swapping a previously set ability. */
	UPROPERTY(EditDefaultsOnly)
	FText SwapAbilityButtonText;
	
};
