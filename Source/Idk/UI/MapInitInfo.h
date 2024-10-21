// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Math/TransformCalculus2D.h>
#include <Widgets/Layout/Anchors.h>

class UTextureRenderTarget2D;

/** Information representing a room on the map. */
struct FMapRoomInfo final
{
public:
	/**
	 * @param RoomType	Type of the room. 
	 * @param RoomPos	Position of the room on the map.
	 */
	FMapRoomInfo(const FGameplayTag& RoomType, FVector2D&& RoomPos);

	/** Get the type of the room. */
	UE_NODISCARD FGameplayTag GetRoomType() const;

	/** Get the position of the room on the map. */
	UE_NODISCARD FVector2D GetRoomPos() const;

private:
	/** Type of the room. */
	FGameplayTag RoomType;

	/** Position of the room on the map. */
	FVector2D RoomPos;
};

/** Information representing a pair of portals on the map. */
struct FMapPortalInfo final
{
public:
	/**
	 * @param HallType	Type of hall the portal connects to.	
	 * @param bLeft		Whether the portal entrance is on the left or right side of the map. 
	 * @param Src		Position of the entrance portal on the map.
	 * @param Dest		Position of the exit portal on the map. 
	 */
	FMapPortalInfo(const FGameplayTag& HallType, const bool bLeft, FVector2D&& Src, FVector2D&& Dest);

	/** Get the type of hall the portal connects to. */
	UE_NODISCARD FGameplayTag GetHallType() const;

	/** Check if the portal entrance is on the left or right side of the map. */
	UE_NODISCARD bool IsOnLeftSide() const;

	/** Get the position of the entrance portal on the map. */
	UE_NODISCARD FVector2D GetSource() const;

	/** Get the position of the exit portal on the map. */
	UE_NODISCARD FVector2D GetDestination() const;

private:
	/** Type of hall the portal connects to. */
	FGameplayTag HallType;

	/** Whether the portal entrance is on the left or right side of the map. */
	bool bLeft;

	/** Position of the entrance portal on the map. */
	FVector2D Source;

	/** Position of the exit portal on the map. */
	FVector2D Destination;
};

/** Information used to initialize the map widget. @see UMapWidget */
struct FMapInitInfo final
{
public:
	/**
	 * @param Image			Overhead picture of the map. 
	 * @param RelativeScale	Scale of the map image relative to the physical map. 
	 * @param OriginAnchor	Anchor at the origin of the map (the center of the spawn room).
	 * @param HallWidth		Width of a hall (in cm).
	 */
	FMapInitInfo(UTextureRenderTarget2D& Image, const FScale2d& RelativeScale, const FAnchors& OriginAnchor, const double HallWidth);

	/**
	 * Add a room to the map.
	 * 
	 * @param RoomInfo	Information representing a room. @see FMapRoomInfo 
	 */
	void AddArena(FMapRoomInfo&& RoomInfo);

	/**
	 * Add a pair of portals to the map.

	 * @param PortalInfo	Information representing a pair of portals. @see FMapPortalInfo 
	 */
	void AddPortal(FMapPortalInfo&& PortalInfo);

	/** Get the overhead picture of the map. */
	UE_NODISCARD UTextureRenderTarget2D& GetImage() const;

	/** Get the scale of the map image relative to the physical map. */
	UE_NODISCARD FScale2d GetRelativeScale() const;

	/** Get the anchor at the origin of the map (the center of the spawn room). */
	UE_NODISCARD FAnchors GetOriginAnchor() const;

	/** Get information about all the rooms on the map. */
	UE_NODISCARD const TArray<FMapRoomInfo>& GetRooms() const;

	/** Get information about all the pairs of portals on the map. */
	UE_NODISCARD const TArray<FMapPortalInfo>& GetPortals() const;

	/** Get the width of a hall (in cm). */
	UE_NODISCARD double GetHallWidth() const;

protected:
	/** Overhead picture of the map. */
	UTextureRenderTarget2D& Image;

	/** Scale of the map image relative to the physical map. */
	FScale2d RelativeScale;

	/** Anchor at the origin of the map (the center of the spawn room). */
	FAnchors OriginAnchor;

	/** Information about all the rooms on the map. @see FMapRoomInfo */
	TArray<FMapRoomInfo> Rooms;

	/** Information about all the pairs of portals on the map. @see FMapPortalInfo */
	TArray<FMapPortalInfo> Portals;

	/** Width of a hall (in cm). */
	double HallWidth = 0.0;
};

