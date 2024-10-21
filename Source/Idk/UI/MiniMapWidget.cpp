// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/MiniMapWidget.h"

#include "Idk/UI/MapInitInfo.h"
#include "Idk/UI/MapWidget.h"
#include <Components/CanvasPanelSlot.h>
#include <Components/ScaleBox.h>
#include <Math/MathFwd.h>
#include <Templates/Casts.h>
#include <Widgets/Layout/Anchors.h>
#include <Widgets/Layout/SScaleBox.h>

void UMiniMapWidget::NativeConstruct()
{
	ScaleBox->SetStretch(EStretch::UserSpecifiedWithClipping);
}

void UMiniMapWidget::InitMapWidget(const FMapInitInfo& MapInitInfo)
{
	MapWidget->Init(MapInitInfo);

	MapSlot = CastChecked<UCanvasPanelSlot>(MapWidget->Slot);
	MapSlot->SetAnchors(FAnchors(0.5));
	MapSlot->SetAlignment(MapInitInfo.GetOriginAnchor().Minimum);
	MapSlot->SetPosition(FVector2D(0.0));

	ScaleBox->SetUserSpecifiedScale(MapInitInfo.GetRelativeScale().Inverse().GetVector().X);
}

void UMiniMapWidget::UpdatePlayerPosition(const FVector2D& PlayerPos)
{
	const FVector2D& ScaledPlayerPos = MapWidget->UpdatePlayerPosition(PlayerPos);

	// Keep the player in the center of the map
	MapSlot->SetPosition(-ScaledPlayerPos);
}
