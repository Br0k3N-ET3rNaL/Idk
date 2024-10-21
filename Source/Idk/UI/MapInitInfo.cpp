// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/MapInitInfo.h"

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <Math/MathFwd.h>
#include <Math/TransformCalculus2D.h>
#include <Templates/UnrealTemplate.h>
#include <Widgets/Layout/Anchors.h>

FMapRoomInfo::FMapRoomInfo(const FGameplayTag& RoomType, FVector2D&& RoomPos)
	: RoomType(RoomType), RoomPos(MoveTemp(RoomPos))
{
}

FGameplayTag FMapRoomInfo::GetRoomType() const
{
	return RoomType;
}

FVector2D FMapRoomInfo::GetRoomPos() const
{
	return RoomPos;
}

FMapPortalInfo::FMapPortalInfo(const FGameplayTag& HallType, const bool bLeft, FVector2D&& Src, FVector2D&& Dest)
	: HallType(HallType), bLeft(bLeft), Source(MoveTemp(Src)), Destination(MoveTemp(Dest))
{
}

FGameplayTag FMapPortalInfo::GetHallType() const
{
	return HallType;
}

bool FMapPortalInfo::IsOnLeftSide() const
{
	return bLeft;
}

FVector2D FMapPortalInfo::GetSource() const
{
	return Source;
}

FVector2D FMapPortalInfo::GetDestination() const
{
	return Destination;
}

FMapInitInfo::FMapInitInfo(UTextureRenderTarget2D& Image, const FScale2d& RelativeScale, const FAnchors& OriginAnchor, const double HallWidth)
	: Image(Image), RelativeScale(RelativeScale), OriginAnchor(OriginAnchor), HallWidth(HallWidth)
{
}

void FMapInitInfo::AddArena(FMapRoomInfo&& RoomInfo)
{
	Rooms.Add(MoveTemp(RoomInfo));
}

void FMapInitInfo::AddPortal(FMapPortalInfo&& PortalInfo)
{
	Portals.Add(MoveTemp(PortalInfo));
}

UTextureRenderTarget2D& FMapInitInfo::GetImage() const
{
	return Image;
}

FScale2d FMapInitInfo::GetRelativeScale() const
{
	return RelativeScale;
}

FAnchors FMapInitInfo::GetOriginAnchor() const
{
	return OriginAnchor;
}

const TArray<FMapRoomInfo>& FMapInitInfo::GetRooms() const
{
	return Rooms;
}

const TArray<FMapPortalInfo>& FMapInitInfo::GetPortals() const
{
	return Portals;
}

double FMapInitInfo::GetHallWidth() const
{
	return HallWidth;
}

