// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>
#include <Misc/EnumClassFlags.h>
#include <UObject/ObjectMacros.h>

#include "ConditionComparator.generated.h"

/** Bitflag representing the type of comparison for complex effects. */
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EConditionComparator : uint8
{
	None = 0 UMETA(Hidden),
	LessThan = 1 << 0,
	GreaterThan = 1 << 1,
	Equal = 1 << 2,
};
ENUM_CLASS_FLAGS(EConditionComparator);
