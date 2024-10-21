// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/MapWidget.h"

#include "Idk/GameSystems/RoomTypes.h"
#include "Idk/UI/MapInitInfo.h"
#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetTree.h>
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <Components/Image.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <Engine/TextureRenderTarget2D.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Math/Color.h>
#include <Math/MathFwd.h>
#include <Math/TransformCalculus2D.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <PaperSprite.h>
#include <Styling/SlateBrush.h>
#include <Templates/Casts.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>
#include <Widgets/Layout/Anchors.h>

UPaperSprite& FMapIconInfo::GetIcon()
{
	return *Icon.LoadSynchronous();
}

const UPaperSprite& FMapIconInfo::GetIcon() const
{
	return *Icon.LoadSynchronous();
}

const FText& FMapIconInfo::GetDisplayName() const
{
	return DisplayName;
}

bool FMapIconInfo::IsIconSet() const
{
	return !Icon.IsNull();
}

UMapWidget::UMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TArray<FGameplayTag> RoomTypes;

	FRoomTypes::GetRoomTypes(RoomTypes);

	for (const FGameplayTag& RoomType : RoomTypes)
	{
		RoomIconInfo.Add(RoomType);
	}

	RightPortalTint = FLinearColor(0.0, 1.0, 1.0);
}

#if WITH_EDITOR
EDataValidationResult UMapWidget::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PortalEntranceIcon.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Portal entrance icon is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (PortalExitIcon.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Portal exit icon is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	for (const TPair<FGameplayTag, FMapIconInfo>& IconInfo : RoomIconInfo)
	{
		if (!IconInfo.Value.IsIconSet())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Icon not set for %s."), *IconInfo.Key.ToString())));

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

void UMapWidget::Init(const FMapInitInfo& MapInitInfo)
{
	check(!MapMaterial.IsNull());

	UMaterialInstanceDynamic* MapMaterialInst =
		UMaterialInstanceDynamic::Create(MapMaterial.LoadSynchronous(), this);

	// Picture of the actual map
	UTextureRenderTarget2D& Image = MapInitInfo.GetImage();

	MapMaterialInst->SetTextureParameterValue(MapTextureParam, &Image);

	MapImage->SetBrushFromMaterial(MapMaterialInst);

	// Get the created brush
	FSlateBrush Brush = MapImage->GetBrush();

	// Adjust the size
	Brush.SetImageSize(FVector2D(Image.SizeX, Image.SizeY));

	// Re-set the brush
	MapImage->SetBrush(Brush);

	Scale = MapInitInfo.GetRelativeScale();
	OriginAnchor = MapInitInfo.GetOriginAnchor();

	const FVector2D IconSize = Scale.TransformVector(FVector2D(MapInitInfo.GetHallWidth() / 2.0));

	PlayerIconSlot = CastChecked<UCanvasPanelSlot>(PlayerIcon->Slot);
	PlayerIconSlot->SetAnchors(OriginAnchor);
	PlayerIconSlot->SetAlignment(IconAlignment);
	PlayerIconSlot->SetAutoSize(false);
	PlayerIconSlot->SetSize(IconSize);
	PlayerIconSlot->SetPosition(Scale.TransformPoint(FVector2D(0.0)));

	for (const FMapRoomInfo& Room : MapInitInfo.GetRooms())
	{
		AddRoomIcon(Room, IconSize);
	}

	for (const FMapPortalInfo& Portal : MapInitInfo.GetPortals())
	{
		AddPortalIcons(Portal, IconSize);
	}

	IconOverlay->InvalidateLayoutAndVolatility();
}

FVector2D UMapWidget::UpdatePlayerPosition(const FVector2D& PlayerPos)
{
	FVector2D ScaledPlayerPos = Scale.TransformPoint(PlayerPos);
	PlayerIconSlot->SetPosition(ScaledPlayerPos);

	return ScaledPlayerPos;
}

TMap<FGameplayTag, FMapIconInfo>& UMapWidget::GetRoomIconInfo()
{
	return RoomIconInfo;
}

UPaperSprite& UMapWidget::GetPortalEntranceIcon()
{
	return *PortalEntranceIcon.LoadSynchronous();
}

UPaperSprite& UMapWidget::GetPortalExitIcon()
{
	return *PortalExitIcon.LoadSynchronous();
}

void UMapWidget::AddRoomIcon(const FMapRoomInfo& RoomInfo, const FVector2D& IconSize)
{
	if (RoomIconInfo.Contains(RoomInfo.GetRoomType()))
	{
		AddIconToMap(RoomIconInfo[RoomInfo.GetRoomType()].GetIcon(), Scale.TransformPoint(RoomInfo.GetRoomPos()), IconSize);
	}
}

void UMapWidget::AddPortalIcons(const FMapPortalInfo& PortalInfo, const FVector2D& IconSize)
{
	const FLinearColor PortalTint = (PortalInfo.IsOnLeftSide()) ? FLinearColor::White : RightPortalTint;

	UPaperSprite& SrcSprite = *PortalEntranceIcon.LoadSynchronous();

	const double SrcIconHalfWidth = SrcSprite.GetRenderBounds().BoxExtent.X;// SrcSprite.GetSourceSize().X;
	const FVector2D SrcPos = Scale.TransformPoint(PortalInfo.GetSource());

	AddIconToMap(SrcSprite, SrcPos, IconSize, PortalTint);

	UPaperSprite& TypeSprite = RoomIconInfo[PortalInfo.GetHallType()].GetIcon();

	const double TypeIconHalfWidth = TypeSprite.GetRenderBounds().BoxExtent.X;
	const double TypeIconXOffset = SrcIconHalfWidth + TypeIconHalfWidth;
	const FVector2D TypeIconOffset = FVector2D(TypeIconXOffset, 0.0);

	AddIconToMap(TypeSprite, SrcPos + TypeIconOffset, IconSize);

	AddIconToMap(*PortalExitIcon.LoadSynchronous(), Scale.TransformPoint(PortalInfo.GetDestination()), IconSize, PortalTint);
}

void UMapWidget::AddIconToMap(UPaperSprite& Sprite, const FVector2D& Pos, const FVector2D& IconSize, const FLinearColor& Tint)
{
	UImage* Icon = WidgetTree->ConstructWidget<UImage>();

	Icon->SetBrushFromAtlasInterface(&Sprite);
	Icon->SetBrushTintColor(Tint);

	UCanvasPanelSlot* CanvasSlot = IconOverlay->AddChildToCanvas(Icon);

	CanvasSlot->SetAnchors(OriginAnchor);
	CanvasSlot->SetAlignment(IconAlignment);
	CanvasSlot->SetAutoSize(false);
	CanvasSlot->SetSize(IconSize);
	CanvasSlot->SetPosition(Pos);
}
