// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <HAL/Platform.h>
#include <Misc/EnumRange.h>
#include <UObject/ObjectMacros.h>

#include "PlayerClasses.generated.h"

/** Enum representing every player class. */
UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	None UMETA(Hidden),
	Knight,
	Mage,
	Num UMETA(Hidden),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EPlayerClass, uint8(EPlayerClass::None) + 1, uint8(EPlayerClass::Num) - 1);
