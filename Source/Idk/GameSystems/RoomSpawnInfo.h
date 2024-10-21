// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>

/** Information used to spawn a room. */
struct FRoomSpawnInfo final
{
	friend class URoomTree;

public:
	FRoomSpawnInfo() = default;
	
	/**
	 * @param Seed	Seed used by the room.
	 * @param Type	Type of the room.
	 * @param Level	Level of the room.
	 * @param Pos	Position of the room.
	 */
	FRoomSpawnInfo(const int32 Seed, const FGameplayTag Type, const uint8 Level, const FVector& Pos = FVector::ZeroVector);

	/** Get the seed used by the room. */
	UE_NODISCARD int32 GetSeed() const;

	/** Get the type of the room. */
	UE_NODISCARD FGameplayTag GetType() const;

	/** Get the level of the room. */
	UE_NODISCARD uint8 GetLevel() const;

	/** Get the position of the room. */
	UE_NODISCARD FVector GetPos() const;

	/** Get the angles (yaw) in radians for gaps in room walls. */
	UE_NODISCARD const TArray<double>& GetGapAngles() const;

	/** Get the yaw of the room. */
	UE_NODISCARD double GetAngle() const;

private:
	/** Seed used by room. */
	int32 Seed = 0;

	/** Type of room. */
	FGameplayTag Type = FGameplayTag::EmptyTag;

	/** Level of the room. */
	uint8 Level = 0;

	/** Position of the room. */
	FVector Pos;

	/** Angles (yaw) in radians for gaps in room walls. */
	TArray<double> GapAngles;

	/** Yaw of the room. */
	double Angle = 0.0;
};
