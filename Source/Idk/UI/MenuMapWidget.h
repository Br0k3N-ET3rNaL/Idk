// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CommonActivatableWidget.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Layout/Margin.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "MenuMapWidget.generated.h"

class UCanvasPanelSlot;
class UContentWidget;
class UGridPanel;
class UMapWidget;
class UPaperSprite;
class UScaleBox;
struct FMapInitInfo;

/** Widget that displays the map in the menu while paused. */
UCLASS(Abstract)
class UMenuMapWidget final : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UMenuMapWidget();

	//~ Begin UUserWidget Interface.
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

	/**
	 * Initialize the map widget.
	 * 
	 * @param MapInitInfo	Information used to initialize the map. @see FMapInitInfo 
	 */
	void InitMapWidget(const FMapInitInfo& MapInitInfo);

	/** Update the player's current position on the map. */
	void UpdatePlayerPosition(const FVector2D& PlayerPos);

	/**
	 * Update the current map zoom.
	 * 
	 * @param InputValue	Value of the zoom input. 
	 */
	void Zoom(const float InputValue);

	/**
	 * Update the current map panning.
	 * 
	 * @param InputValue	Value of the pan input. 
	 */
	void Pan(const FVector2D& InputValue);

private:
	/** Initialize the legend for the map. */
	void InitLegend();

	/**
	 * Add a row with an icon and display name to the map legend.
	 * 
	 * @param InOutRow		Current row. 
	 * @param Icon			Icon to use.
	 * @param DisplayName	Text to display for the icon.
	 */
	void AddEntryToLegend(int32& InOutRow, UPaperSprite& Icon, const FText& DisplayName);

	/** Map widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMapWidget> MapWidget;

	/** Scale box used for zooming in/out. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> ScaleBox;

	/** Grid panel containing for the legend. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGridPanel> LegendGrid;

	/** Slot of the map in the canvas panel. */
	TObjectPtr<UCanvasPanelSlot> MapSlot;

	/** Text to display for the portal entrance icon in the map legend. */
	UPROPERTY(EditDefaultsOnly)
	FText PortalEntranceDisplayName;

	/** Text to display for the portal exit icon in the map legend. */
	UPROPERTY(EditDefaultsOnly)
	FText PortalExitDisplayName;

	/** Padding to use for icons in the map legend. */
	UPROPERTY(EditDefaultsOnly)
	FMargin LegendIconPadding = FMargin(0.0, 0.0, 10.0, 10.0);

	/** Multiplied by the input value to get the change to the zoom amount. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.01f, ClampMax = 10.f))
	float ZoomMultiplier = 0.1f;

	/** Minimum zoom amount. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.01f, ClampMax = 10.f))
	float MinZoom = 0.25f;

	/** Maximum zoom amount. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.01f, ClampMax = 10.f))
	float MaxZoom = 5.f;

	/** Current zoom amount. */
	UPROPERTY(VisibleInstanceOnly)
	float CurrentZoom = 1.f;

	/** Value multiplied by the input value to get the change in panning. */
	UPROPERTY(EditDefaultsOnly)
	double PanMultiplier = 4.0;

	/** Minimum panning. */
	FVector2D MinPan;

	/** Maximum panning. */
	FVector2D MaxPan;

};
