// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/RoomSpawnInfo.h"

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>

FRoomSpawnInfo::FRoomSpawnInfo(const int32 Seed, const FGameplayTag Type, const uint8 Level, const FVector& Pos)
	: Seed(Seed), Type(Type), Level(Level), Pos(Pos)
{
}

FGameplayTag FRoomSpawnInfo::GetType() const
{
	return Type;
}

uint8 FRoomSpawnInfo::GetLevel() const
{
	return Level;
}

int32 FRoomSpawnInfo::GetSeed() const
{
	return Seed;
}

FVector FRoomSpawnInfo::GetPos() const
{
	return Pos;
}

const TArray<double>& FRoomSpawnInfo::GetGapAngles() const
{
	return GapAngles;
}

double FRoomSpawnInfo::GetAngle() const
{
	return Angle;
}
