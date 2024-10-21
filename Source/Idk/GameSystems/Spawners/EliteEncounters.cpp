// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/EliteEncounters.h"

#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <Containers/Array.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>

TSubclassOf<AEliteEnemyCharacter> FEliteEncounter::GetEliteClass() const
{
	check(!EliteClass.IsNull());

	return EliteClass.LoadSynchronous();
}

const TArray<FEnemySpawnInfo>& FEliteEncounter::GetAdds() const
{
	return Adds;
}
