// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/BossEncounters.h"

#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <Containers/Array.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>

TSubclassOf<ABossEnemyCharacter> FBossEncounter::GetBossClass() const
{
	check(!BossClass.IsNull());

	return BossClass.LoadSynchronous();
}

const TArray<FEnemySpawnInfo>& FBossEncounter::GetAdds() const
{
	return Adds;
}
