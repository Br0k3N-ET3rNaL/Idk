// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "HUDWidget.generated.h"

class AIdkPlayerController;
class FText;
class UAbilityChoiceWidget;
class UAbilitySwapWidget;
class UAbilityWidget;
class UAttributeDisplayWidget;
class UBackgroundBlur;
class UExperienceWidget;
class UGameEndWidget;
class UGoldWidget;
class UHealthAndStatusBarWidget;
class UImage;
class UInventoryWidget;
class UMapWidget;
class UMenuMapWidget;
class UMiniMapWidget;
class UPauseMenuWidget;
class UTempMessageWidget;
class UWidget;
struct FMapInitInfo;
struct FInputActionValue;

/** Contains widgets used by the player. */
struct FPlayerWidgets final
{
public:
	/** Check if all widgets are set. */
	UE_NODISCARD bool IsValid() const;

	UAbilityWidget* AbilityWidget1 = nullptr;
	UAbilityWidget* AbilityWidget2 = nullptr;
	UInventoryWidget* InventoryWidget = nullptr;
	UExperienceWidget* ExperienceWidget = nullptr;
	UGoldWidget* GoldWidget = nullptr;
	UAbilityChoiceWidget* AbilityChoiceWidget = nullptr;
	UAbilitySwapWidget* AbilitySwapWidget = nullptr;
	UAttributeDisplayWidget* AttributeDisplayWidget = nullptr;
};

/** Represents openable menus. */
enum class EMenu : uint8
{
	None,
	Inventory,
	MenuMap,
	PauseMenu,
	AbilityChoice,
	AbilitySwap,
};

/** Widget displayed to the player during gameplay. */
UCLASS(Abstract)
class UHUDWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	//~ Begin UUserWidget Interface.
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Initialize the HUD widget. */
	void Init();

	/**
	 * Initialize the map widget.
	 * 
	 * @param MapInitInfo	 Information used to initialize the map widget. @see FMapInitInfo
	 */
	void InitMapWidget(const FMapInitInfo& MapInitInfo);

	/**
	 * Switch the currently open menu.
	 * 
	 * @param MenuToSwitchTo	Menu to switch to.
	 * @param bToggle			Whether to close the currenlty open menu if it's the same as MenuToSwitchTo.
	 */
	void SwitchOpenMenu(const EMenu MenuToSwitchTo, const bool bToggle);

	/** Event called when the escape key is pressed. */
	void OnEscapeKeyPressed();

	/** Get widgets used by the player. @see FPlayerWidgets */
	void GetPlayerWidgets(FPlayerWidgets& OutPlayerWidgets) const;

	/** Get the health and status bar widget. */
	UE_NODISCARD UHealthAndStatusBarWidget& GetHealthAndStatusBarWidget() const;

	/** Update the player's position on the map widget. */
	void UpdatePlayerPosition(const FVector2D& PlayerPos);

	/**
	 * Update the current map zoom.
	 * 
	 * @param InputValue	Value of the zoom input. 
	 */
	void UpdateMapZoom(const float InputValue);

	/**
	 * Update the current map pan.
	 * 
	 * @param InputValue	Value of the pan input.
	 */
	void UpdateMapPan(const FVector2D& InputValue);

	/** Show a temporary message to the player. */
	void ShowTempMessage(const FText& Message);

	/**
	 * Show the game end screen.
	 * 
	 * @param bWon	Whether the player won or lost. 
	 */
	void ShowGameEndScreen(const bool bWon);

private:
	/** Set the visibility of the inventory menu. */
	void SetInventoryVisibility(const bool bVisible);

	/** Set whether the game is paused. */
	void SetMenuPause(const bool bPause);

	/** Widget for the player's first ability. @see UAbilityWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityWidget> AbilityWidget1;

	/** Widget for the player's second ability. @see UAbilityWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityWidget> AbilityWidget2;

	/** Widget that displays the health and status effect bars. @see UHealthAndStatusBarWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHealthAndStatusBarWidget> HealthAndStatusBarWidget;

	/** Blurs the background when a menu is open. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBackgroundBlur> Blur;

	/** Widget that displays the player's inventory. @see UInventoryWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	/** Widget that displays the map menu. @see UMenuMapWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuMapWidget> MenuMapWidget;

	/** Widget that displays a smaller version of the map when not in menus. @see UMiniMapWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMiniMapWidget> MiniMapWidget;

	/** Widget that displays the player's current experience and level. @see UExperienceWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UExperienceWidget> ExperienceWidget;

	/** Widget that displays the player's current gold. @see UGoldWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGoldWidget> GoldWidget;

	/** Widget that allows the player to return to the main menu, exit the game, or adjust settings. @see UPauseMenuWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPauseMenuWidget> PauseMenuWidget;

	/** Widget that allows the player to choose between abilities. @see UAbilityChoiceWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityChoiceWidget> AbilityChoiceWidget;

	/** Widget that allows the player to swap equipped abilities for a new ability. @see UAbilitySwapWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilitySwapWidget> AbilitySwapWidget;

	/** Widget that displays the current value of the player's attributes. @see UAttributeDisplayWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAttributeDisplayWidget> AttributeDisplayWidget;

	/** Widget that is used to show temporary messages to the player. @see UTempMessageWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTempMessageWidget> TempMessageWidget;

	/** Widget shown to the player when the game ends. @see UGameEndWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGameEndWidget> GameEndWidget;

	/** Group of widgets along the top edge of the screen. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> TopBar;

	/** Crosshair widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;

	/** Controller for the current player. */
	TObjectPtr<AIdkPlayerController> PlayerController;

	/** Currently open menu. */
	EMenu OpenMenu = EMenu::None;

	/** Whether the game has ended. */
	bool bGameEnd = false;

};
