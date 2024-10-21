// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/PlayerClasses.h"
#include <Blueprint/UserWidget.h>
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "CharacterSelectWidget.generated.h"

DECLARE_DELEGATE_TwoParams(FOnSelectedCharacterChangedDelegate, EPlayerClass, EPlayerClass);
DECLARE_DELEGATE_OneParam(FOnCharacterChosenDelegate, EPlayerClass);

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UButton;
class UCharacterSelectInfoWidget;
class UCommonButton;
class UCommonWidgetCarousel;

/** Information about a playable class. */
USTRUCT()
struct FCharacterDisplayInfo final
{
	GENERATED_BODY()

public:
	FCharacterDisplayInfo() = default;

	/**
	 * @param PlayerClass	Which playable class this represents. 
	 * @param Name			Name of the class.
	 * @param Description	Description of the class.
	 */
	FCharacterDisplayInfo(const EPlayerClass PlayerClass, const FText& Name, const FText& Description);

	/** Get which playable class this represents. */
	UE_NODISCARD EPlayerClass GetPlayerClass() const;

	/** Get the name of the class. */
	UE_NODISCARD const FText& GetName() const;

	/** Get the description of the class. */
	UE_NODISCARD const FText& GetDescription() const;

	/** Check if there is another class before this one. */
	UE_NODISCARD bool CanSelectPrevious() const;

	/** Check if there is another class after this one. */
	UE_NODISCARD bool CanSelectNext() const;

private:
	/** Which playable class this represents. */
	UPROPERTY(VisibleInstanceOnly)
	EPlayerClass PlayerClass = EPlayerClass();

	/** Name of the class. */
	UPROPERTY(VisibleInstanceOnly)
	FText Name;

	/** Description of the class. */
	UPROPERTY(VisibleInstanceOnly, meta = (MultiLine))
	FText Description;

	/** Whether there is another class before this one. */
	UPROPERTY(VisibleInstanceOnly)
	bool bCanSelectPrevious = false;

	/** Whether there is another class after this one. */
	UPROPERTY(VisibleInstanceOnly)
	bool bCanSelectNext = false;

};

/** Widget that allows the player to choose a class. */
UCLASS(Abstract)
class UCharacterSelectWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the character select widget.
	 * 
	 * @param InCharacterDisplayInfo	Information for each selectable class. 
	 */
	void Init(const TArray<FCharacterDisplayInfo>& InCharacterDisplayInfo);

	/** Sets whether the buttons are enabled or not. */
	void SetButtonsEnabled(const bool bEnabled);

	/**
	 * Delegate called when the currently selected class is changed.
	 * 
	 * @param PreviousClass	Previously selected class.
	 * @param CurrentClass	Currently selected class.
	 */
	FOnSelectedCharacterChangedDelegate OnSelectedCharacterChangedDelegate;

	/**
	 * Delegate called when a class is chosen by the player.
	 * 
	 * @param Class	Chosen class.
	 */
	FOnCharacterChosenDelegate OnCharacterChosenDelegate;

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Get the class of the currently selected character. */
	UE_NODISCARD EPlayerClass GetCurrentPlayerClass() const;

	/** Update buttons based on currently selected class. */
	void UpdateButtons();

	/** Event called when the previous button is clicked. */
	UFUNCTION()
	void OnPreviousButtonClicked();

	/** Event called when the next button is clicked. */
	UFUNCTION()
	void OnNextButtonClicked();

	/** Event called when the confirm button is clicked. */
	UFUNCTION()
	void OnConfirmButtonClicked();

	/** Button that switches to the previous playable class. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> PreviousButton;

	/** Button that switches to the next playable class. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> NextButton;

	/** Button the causes the currenlty selected class to be chosen. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButton> ConfirmButton;

	/** Displays the information for each playable class. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonWidgetCarousel> CharacterInfoCarousel;

	/** Class for the widget that displays information about the currently selected class. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<UCharacterSelectInfoWidget> InfoWidgetClass;

	/** Information about each playable class. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<FCharacterDisplayInfo> CharacterDisplayInfo;
	
};
