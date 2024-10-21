// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/MenuMapWidget.h"

#include "Idk/UI/MapInitInfo.h"
#include "Idk/UI/MapWidget.h"
#include <Blueprint/WidgetTree.h>
#include <CommonActivatableWidget.h>
#include <Components/CanvasPanelSlot.h>
#include <Components/GridPanel.h>
#include <Components/GridSlot.h>
#include <Components/Image.h>
#include <Components/ScaleBox.h>
#include <Components/TextBlock.h>
#include <Containers/Map.h>
#include <Engine/TextureRenderTarget2D.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Layout/Margin.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <PaperSprite.h>
#include <Templates/Casts.h>
#include <Widgets/Layout/SScaleBox.h>

UMenuMapWidget::UMenuMapWidget()
{
	PortalEntranceDisplayName = FText::FromString(TEXT("Portal Entrance"));
	PortalExitDisplayName = FText::FromString(TEXT("Portal Exit"));
}

void UMenuMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScaleBox->SetStretch(EStretch::UserSpecifiedWithClipping);

	MapSlot = CastChecked<UCanvasPanelSlot>(MapWidget->Slot);
}

void UMenuMapWidget::InitMapWidget(const FMapInitInfo& MapInitInfo)
{
	MapWidget->Init(MapInitInfo);

	const double HalfSizeX = MapInitInfo.GetImage().SizeX / 2.0;
	const double HalfSizeY = MapInitInfo.GetImage().SizeY / 2.0;

	// Set  minimum and maximum panning values based on map size
	MinPan = FVector2D(-HalfSizeX, -HalfSizeY);
	MaxPan = FVector2D(HalfSizeX, HalfSizeY);

	InitLegend();
}

void UMenuMapWidget::UpdatePlayerPosition(const FVector2D& PlayerPos)
{
	MapWidget->UpdatePlayerPosition(PlayerPos);
}

void UMenuMapWidget::Zoom(const float InputValue)
{
	CurrentZoom = FMath::Clamp<float>((InputValue * ZoomMultiplier) + CurrentZoom, MinZoom, MaxZoom);

	ScaleBox->SetUserSpecifiedScale(CurrentZoom);
}

void UMenuMapWidget::Pan(const FVector2D& InputValue)
{
	const FVector2D Input = (InputValue * PanMultiplier) / CurrentZoom;

	FVector2D Pos = MapSlot->GetPosition() + Input;

	Pos.X = FMath::Clamp(Pos.X, MinPan.X, MaxPan.X);
	Pos.Y = FMath::Clamp(Pos.Y, MinPan.Y, MaxPan.Y);

	MapSlot->SetPosition(Pos);
}

void UMenuMapWidget::InitLegend()
{
	TMap<FGameplayTag, FMapIconInfo>& RoomIconInfo = MapWidget->GetRoomIconInfo();

	int32 Row = 0;

	// Add entries to the legend for each room type
	for (TPair<FGameplayTag, FMapIconInfo>& IconInfo : RoomIconInfo)
	{
		AddEntryToLegend(Row, IconInfo.Value.GetIcon(), IconInfo.Value.GetDisplayName());
	}

	AddEntryToLegend(Row, MapWidget->GetPortalEntranceIcon(), PortalEntranceDisplayName);
	AddEntryToLegend(Row, MapWidget->GetPortalExitIcon(), PortalExitDisplayName);
}

void UMenuMapWidget::AddEntryToLegend(int32& InOutRow, UPaperSprite& Icon, const FText& DisplayName)
{
	// Add the icon
	UImage* IconWidget = WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetBrushFromAtlasInterface(&Icon);
	UGridSlot* IconSlot = LegendGrid->AddChildToGrid(IconWidget, InOutRow, 0);
	IconSlot->SetPadding(LegendIconPadding);

	// Add the text
	UTextBlock* RoomTypeName = WidgetTree->ConstructWidget<UTextBlock>();
	RoomTypeName->SetText(DisplayName);
	LegendGrid->AddChildToGrid(RoomTypeName, InOutRow, 1);

	++InOutRow;
}
