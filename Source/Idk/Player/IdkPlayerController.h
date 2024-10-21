// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/PlayerController.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "IdkPlayerController.generated.h"

class APawn;
class UAbilityWidget;
class UEnhancedInputUserSettings;
class UHealhAndStatusBarWidget;
class UHUDWidget;
class UInputMappingContext;
class UInventoryWidget;
class UUserWidget;

/** Controller used by the player character. */
UCLASS(Abstract, Blueprintable)
class AIdkPlayerController final : public APlayerController
{
	GENERATED_BODY()
	
public:
	AIdkPlayerController();

	/** Get the current HUD widget. */
	UE_NODISCARD UHUDWidget& GetHUDWidget();

	/**
	 * Set the current input mode for gameplay.
	 * 
	 * @param bChangeInputContext	Whether to change the applied input contexts. 
	 */
	void SetInputModeToGameplay(const bool bChangeInputContext = true);

	/**
	 * Set the current input mode for menus.
	 * 
	 * @param bChangeInputContext	Whether to change the applied input contexts.  
	 */
	void SetInputModeToMenu(const bool bChangeInputContext = true);

	/** Toggle the visibility of the mouse cursor. */
	void ToggleMouseVisibility();

	/** Get the current input settings. */
	UE_NODISCARD UEnhancedInputUserSettings& GetInputSettings();

	/** Lock input while waiting to return to the main menu or exit the game. */
	void WaitForExit();

private:
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin AController Interface.
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AController Interface

	/** Load or create input settings. */
	void LoadOrCreateInputSettings();

	/** Default input mapping context. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UInputMappingContext> DefaultInputMapping;

	/** Input mapping context to apply while using a menu. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UInputMappingContext> MenuInputMapping;

	/** Input mapping context to apply while using the map. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UInputMappingContext> MapInputMapping;

	/** Widget to use as the cursor when waiting for an operation to complete. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UUserWidget> WaitingMouseCursorWidgetClass;

	/** Current input settings. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UEnhancedInputUserSettings> InputSettings;

	/** Class used to create the HUD widget. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UHUDWidget> HUDWidgetClass;

	/** HUD widget. */
	UPROPERTY()
	TObjectPtr<UHUDWidget> HUDWidget;

	/** Input mode for gameplay. */
	FInputModeGameOnly GameplayInputMode;

	/** Input mode for menus. */
	FInputModeGameAndUI MenuInputMode;

};
