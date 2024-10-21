// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkEnemyCharacter.h"
#include <UObject/ObjectMacros.h>

#include "BossEnemyCharacter.generated.h"

/** Base class for boss enemies. */
UCLASS(Abstract)
class ABossEnemyCharacter : public AIdkEnemyCharacter
{
	GENERATED_BODY()
	
};
