// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "MiniMapWidget.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;
class UMapWidget;
class UScaleBox;
struct FMapInitInfo;

/** Widget that displays the map during gameplay. */
UCLASS(Abstract)
class UMiniMapWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
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

private:
	/** Map widget. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMapWidget> MapWidget;

	/** Scale box used to adjust zoom based on map size. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> ScaleBox;

	/** Slot of the map in the canvas panel. */
	TObjectPtr<UCanvasPanelSlot> MapSlot;
};
