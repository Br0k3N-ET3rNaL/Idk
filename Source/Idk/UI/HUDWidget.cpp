// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/HUDWidget.h"

#include "Idk/Player/IdkPlayerController.h"
#include "Idk/UI/AbilityChoiceWidget.h"
#include "Idk/UI/AbilitySwapWidget.h"
#include "Idk/UI/AttributeDisplayWidget.h"
#include "Idk/UI/GameEndWidget.h"
#include "Idk/UI/InventoryWidget.h"
#include "Idk/UI/MapWidget.h"
#include "Idk/UI/MenuMapWidget.h"
#include "Idk/UI/MiniMapWidget.h"
#include "Idk/UI/PauseMenuWidget.h"
#include "Idk/UI/TempMessageWidget.h"
#include <Components/BackgroundBlur.h>
#include <Components/Image.h>
#include <Components/SlateWrapperTypes.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Templates/Casts.h>

bool FPlayerWidgets::IsValid() const
{
	return AbilityWidget1 && AbilityWidget2 && InventoryWidget && ExperienceWidget 
		&& GoldWidget && AbilityChoiceWidget && AbilitySwapWidget && AttributeDisplayWidget;
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetInventoryVisibility(false);
	MenuMapWidget->SetVisibility(ESlateVisibility::Hidden);
	Blur->SetVisibility(ESlateVisibility::Hidden);
	PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	AbilityChoiceWidget->SetVisibility(ESlateVisibility::Hidden);
	AbilitySwapWidget->SetVisibility(ESlateVisibility::Hidden);
	GameEndWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerController = CastChecked<AIdkPlayerController>(GetOwningPlayer());

	PauseMenuWidget->ClosePauseMenuDelegate.BindLambda([&]()
		{
			OnEscapeKeyPressed();
		});
}

void UHUDWidget::Init()
{
	PauseMenuWidget->Init();
}

void UHUDWidget::InitMapWidget(const FMapInitInfo& MapInitInfo)
{
	MiniMapWidget->InitMapWidget(MapInitInfo);
	MenuMapWidget->InitMapWidget(MapInitInfo);
}

void UHUDWidget::SwitchOpenMenu(const EMenu MenuToSwitchTo, const bool bToggle)
{
	// Whether the currently open menu will change
	const bool bWillChangeOpenMenu = MenuToSwitchTo != OpenMenu || bToggle;

	// Whether the currently open menu will switch to a different menu
	const bool bWillSwitchOpenMenu = MenuToSwitchTo != OpenMenu && MenuToSwitchTo != EMenu::None;

	// Whether the currently open menu will be closed
	const bool bWillCloseOpenMenu = MenuToSwitchTo == EMenu::None || (MenuToSwitchTo == OpenMenu && bToggle);

	// Whether the currently open menu can be switched
	const bool bCanSwitchMenu = OpenMenu != EMenu::AbilitySwap && OpenMenu != EMenu::AbilityChoice;

	// Whether the currently open menu can be closed
	const bool bCanCloseMenu = !bGameEnd && (bCanSwitchMenu || MenuToSwitchTo != EMenu::None);

	if (bWillChangeOpenMenu && (bWillSwitchOpenMenu && bCanSwitchMenu) || (bWillCloseOpenMenu && bCanCloseMenu))
	{
		// Whether the currently open menu should be closed
		const bool bCloseMenu = OpenMenu != EMenu::None;

		// Whether another menu will be opened
		const bool bOpenMenu = bWillSwitchOpenMenu;

		if (bCloseMenu)
		{
			switch (OpenMenu)
			{
				case EMenu::Inventory:
					SetInventoryVisibility(false);
					break;
				case EMenu::MenuMap:
					MenuMapWidget->SetVisibility(ESlateVisibility::Hidden);
					MenuMapWidget->DeactivateWidget();
					break;
				case EMenu::PauseMenu:
					PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
					PauseMenuWidget->CloseSettings();
					break;
				case EMenu::AbilityChoice:
					AbilityChoiceWidget->SetVisibility(ESlateVisibility::Hidden);
					break;
				case EMenu::AbilitySwap:
					AbilitySwapWidget->SetVisibility(ESlateVisibility::Hidden);
					break;
				default:
					break;
			}
		}

		if (bOpenMenu)
		{
			OpenMenu = MenuToSwitchTo;

			switch (MenuToSwitchTo)
			{
				case EMenu::Inventory:
					SetInventoryVisibility(true);
					break;
				case EMenu::MenuMap:
					MenuMapWidget->SetVisibility(ESlateVisibility::Visible);
					MenuMapWidget->ActivateWidget();
					break;
				case EMenu::PauseMenu:
					PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
					break;
				case EMenu::AbilityChoice:
					AbilityChoiceWidget->SetVisibility(ESlateVisibility::Visible);
					break;
				case EMenu::AbilitySwap:
					AbilitySwapWidget->SetVisibility(ESlateVisibility::Visible);
					break;
				default:
					break;
			}
		}

		if (bOpenMenu && !bCloseMenu)
		{
			SetMenuPause(true);
			MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
			Blur->SetVisibility(ESlateVisibility::Visible);
			Crosshair->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (!bOpenMenu && bCloseMenu)
		{
			OpenMenu = EMenu::None;
			SetMenuPause(false);
			MiniMapWidget->SetVisibility(ESlateVisibility::Visible);
			Blur->SetVisibility(ESlateVisibility::Hidden);
			Crosshair->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UHUDWidget::OnEscapeKeyPressed()
{
	const bool bTogglePauseMenu = OpenMenu == EMenu::None || OpenMenu == EMenu::PauseMenu;

	const EMenu Menu = (bTogglePauseMenu) ? EMenu::PauseMenu : EMenu::None;

	SwitchOpenMenu(Menu, bTogglePauseMenu);
}

void UHUDWidget::GetPlayerWidgets(FPlayerWidgets& OutPlayerWidgets) const
{
	OutPlayerWidgets.AbilityWidget1 = AbilityWidget1;
	OutPlayerWidgets.AbilityWidget2 = AbilityWidget2;
	OutPlayerWidgets.InventoryWidget = InventoryWidget;
	OutPlayerWidgets.ExperienceWidget = ExperienceWidget;
	OutPlayerWidgets.GoldWidget = GoldWidget;
	OutPlayerWidgets.AbilityChoiceWidget = AbilityChoiceWidget;
	OutPlayerWidgets.AbilitySwapWidget = AbilitySwapWidget;
	OutPlayerWidgets.AttributeDisplayWidget = AttributeDisplayWidget;
}

UHealthAndStatusBarWidget& UHUDWidget::GetHealthAndStatusBarWidget() const
{
	return *HealthAndStatusBarWidget;
}

void UHUDWidget::UpdatePlayerPosition(const FVector2D& PlayerPos)
{
	MiniMapWidget->UpdatePlayerPosition(PlayerPos);
	MenuMapWidget->UpdatePlayerPosition(PlayerPos);
}

void UHUDWidget::UpdateMapZoom(const float InputValue)
{
	MenuMapWidget->Zoom(InputValue);
}

void UHUDWidget::UpdateMapPan(const FVector2D& InputValue)
{
	MenuMapWidget->Pan(InputValue);
}

void UHUDWidget::ShowTempMessage(const FText& Message)
{
	TempMessageWidget->ShowMessage(Message);
}

void UHUDWidget::ShowGameEndScreen(const bool bWon)
{
	SwitchOpenMenu(EMenu::Inventory, false);

	bGameEnd = true;

	GameEndWidget->OnGameEnd(bWon);

	TopBar->SetVisibility(ESlateVisibility::Collapsed);
	GameEndWidget->SetVisibility(ESlateVisibility::Visible);
}

void UHUDWidget::SetInventoryVisibility(const bool bVisible)
{
	const ESlateVisibility NewVisibility = (bVisible) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	InventoryWidget->SetVisibility(NewVisibility);
	AttributeDisplayWidget->SetVisibility(NewVisibility);
}

void UHUDWidget::SetMenuPause(const bool bPause)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bPause);

	if (bPause)
	{
		PlayerController->SetInputModeToMenu();
	}
	else
	{
		PlayerController->SetInputModeToGameplay();
	}
}
