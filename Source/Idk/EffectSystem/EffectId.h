// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <HAL/Platform.h>
#include <Misc/Crc.h>
#include <UObject/ObjectMacros.h>

#include "EffectId.generated.h"

/** Represents an ID used by an effect. */
USTRUCT()
struct FEffectId final
{
	GENERATED_BODY()

public:
	FEffectId() = default;
	FEffectId(const uint8 Id);

	UE_NODISCARD operator uint8() const;
	FEffectId& operator++();
	FEffectId operator++(int);
	FEffectId& operator+=(const uint8 Rhs);

	/** Check whether the specified ID is reserved. */
	UE_NODISCARD static constexpr bool IsReservedId(const FEffectId Id)
	{
		return Id.Id == StackId || Id.Id == ConditionalEffectValueId || Id.Id == PartialEffectPlaceholderId;
	}

	/** Value representing an unset ID. */
	static const uint8 NoId = 0;

	/** Minimum valid ID. */
	static const uint8 MinValidId = 1;

	/** ID that represents the number of stacks of an effect. */
	static const uint8 StackId = 255;

	/** ID that represents the value of the effect that triggered a conditional effect (OnDamage, OnHeal, etc.). */
	static const uint8 ConditionalEffectValueId = 254;

	/** ID that will be swapped for a valid ID when adding a partial effect to another effect. */
	static const uint8 PartialEffectPlaceholderId = 253;

private:
	/** The ID. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1"))
	uint8 Id = NoId;

};

FORCEINLINE uint32 GetTypeHash(const FEffectId& EffectId)
{
	return FCrc::MemCrc32(&EffectId, sizeof(FEffectId));
}

