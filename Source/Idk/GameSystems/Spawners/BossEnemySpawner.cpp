// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/BossEnemySpawner.h"

#include "Idk/Character/BossEnemyCharacter.h"
#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Spawners/BossEncounters.h"
#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>

void ABossEnemySpawner::SetEncounters(const UBossEncounters& InBossEncounters)
{
	BossEncounters = &InBossEncounters;
}

void ABossEnemySpawner::SpawnBoss()
{
	check(BossEncounters);

	const FBossEncounter& Encounter = BossEncounters->Encounters[Rng->RandRange(0, BossEncounters->Encounters.Num() - 1)];

	// Spawn the boss
	SpawnEnemy(Encounter.GetBossClass(), Level);

	++LivingEnemies;

	// Spawn any additional enemies that are a part of the boss encounter
	for (const FEnemySpawnInfo& EnemySpawnInfo : Encounter.GetAdds())
	{
		TSubclassOf<AIdkEnemyCharacter> EnemyClass = EnemySpawnInfo.GetEnemyClass();

		for (uint8 i = 0; i < EnemySpawnInfo.GetNumToSpawn(); ++i)
		{
			SpawnEnemy(EnemyClass, Level);

			++LivingEnemies;
		}
	}
}
