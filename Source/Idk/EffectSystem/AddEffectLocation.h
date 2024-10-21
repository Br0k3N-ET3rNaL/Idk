// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "AddEffectLocation.generated.h"

/** Represents effects that bonus effects can be added to. */
UENUM()
enum class EAddEffectLocation : uint8
{
	None UMETA(Hidden),
	BasicAttack,
	Abilities,
	OnTakeDamage,
	OnHeal,
	OnDeath,
	OnKill,
};

