// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Containers/Map.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/Color.h>
#include <Math/MathFwd.h>
#include <Math/TransformCalculus2D.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>
#include <Widgets/Layout/Anchors.h>

#include "MapWidget.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FObjectInitializer;
class UCanvasPanel;
class UCanvasPanelSlot;
class UImage;
class UMaterialInterface;
class UPaperSprite;
class UTextureRenderTarget2D;
struct FMapInitInfo;
struct FMapPortalInfo;
struct FMapRoomInfo;

/** Information used to display an icon on the map. */
USTRUCT()
struct FMapIconInfo final
{
	GENERATED_BODY()

public:
	/** Get the icon. */
	UE_NODISCARD UPaperSprite& GetIcon();

	/** Get the icon. */
	UE_NODISCARD const UPaperSprite& GetIcon() const;

	/** Get the name displayed in the map legend. */
	UE_NODISCARD const FText& GetDisplayName() const;

	/** Check if the icon is set. */
	UE_NODISCARD bool IsIconSet() const;

private:
	/** Sprite for the icon. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPaperSprite> Icon;

	/** Name displayed in the map legend. */
	UPROPERTY(EditAnywhere)
	FText DisplayName;
};

/** Widget used to display the map. */
UCLASS(Abstract)
class UMapWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	UMapWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the map widget.
	 * 
	 * @param MapInitInfo	Information used to initialize the map. @see FMapInitInfo 
	 */
	void Init(const FMapInitInfo& MapInitInfo);

	/**
	 * Update the player's position on the map.
	 * 
	 * @param PlayerPos	Player's current position. 
	 * @return			The player's position scaled by the scale of the map.
	 */
	FVector2D UpdatePlayerPosition(const FVector2D& PlayerPos);

	/** Get icon information for each room type. @see RoomIconInfo */
	UE_NODISCARD TMap<FGameplayTag, FMapIconInfo>& GetRoomIconInfo();

	/** Get the icon for portal entrances. */
	UE_NODISCARD UPaperSprite& GetPortalEntranceIcon();

	/** Get the icon for portal exits. */
	UE_NODISCARD UPaperSprite& GetPortalExitIcon();
	
private:
	/**
	 * Add an icon for a room.
	 * 
	 * @param RoomInfo	Information about the room to add an icon for. 
	 * @param IconSize	Size to use for the icon.
	 */
	void AddRoomIcon(const FMapRoomInfo& RoomInfo, const FVector2D& IconSize);

	/**
	 * Add icons for a set of portals.
	 * 
	 * @param PortalInfo	Information about the set of portals to add icons for. 
	 * @param IconSize		Size to use for the icons.
	 */
	void AddPortalIcons(const FMapPortalInfo& PortalInfo, const FVector2D& IconSize);

	/**
	 * Add an icon to the map.

	 * @param Sprite	Sprite to use for the icon. 
	 * @param Pos		Position of the icon on the map.
	 * @param IconSize	Size to use for the icon.
	 * @param Tint		Tint to apply to the icon.
	 */
	void AddIconToMap(UPaperSprite& Sprite, const FVector2D& Pos, const FVector2D& IconSize, const FLinearColor& Tint = FLinearColor::White);

	/** Material used to convert a picture of the map into a solid color. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UMaterialInterface> MapMaterial;

	/** Image used to display the map. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapImage;

	/** Widget containing all the map icons. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> IconOverlay;

	/** Image representing the player's position on the map. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIcon;

	/** Maps room types to information used to display the icon for that room type. @see FMapIconInfo */
	UPROPERTY(EditDefaultsOnly, meta = (ReadOnlyKeys, ForceInlineRow))
	TMap<FGameplayTag, FMapIconInfo> RoomIconInfo;

	/** Sprite for portal entrance icons. */
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UPaperSprite> PortalEntranceIcon;

	/** Sprite for portal exit icons. */
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UPaperSprite> PortalExitIcon;

	/** Tint to apply to portals on the right side of the map to distinguish them from portals on the left. */
	UPROPERTY(EditDefaultsOnly)
	FLinearColor RightPortalTint;

	/** Slot containing the icon representing the player. */
	TObjectPtr<UCanvasPanelSlot> PlayerIconSlot;

	/** Scale of the map image relative to the actual map. */
	FScale2d Scale;

	/** Anchor at the origin of the map (the center of the spawn room). */
	FAnchors OriginAnchor;

	/** Alignment to use for icons. */
	const FVector2D IconAlignment = FVector2D(0.5);

	/** Name of the material parameter for the map texture. */
	const FName MapTextureParam = TEXT("MapTexture");
};
