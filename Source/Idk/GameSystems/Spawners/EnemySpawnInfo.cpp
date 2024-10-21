// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>

TSubclassOf<AIdkEnemyCharacter> FEnemySpawnInfo::GetEnemyClass() const
{
	check(!EnemyClass.IsNull());

	return EnemyClass.LoadSynchronous();
}

uint8 FEnemySpawnInfo::GetNumToSpawn() const
{
	return NumToSpawn;
}
