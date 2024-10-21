// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/EliteEnemySpawner.h"

#include "Idk/Character/EliteEnemyCharacter.h"
#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Spawners/EliteEncounters.h"
#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>

void AEliteEnemySpawner::SetEncounters(const UEliteEncounters& InEliteEncounters)
{
	EliteEncounters = &InEliteEncounters;
}

void AEliteEnemySpawner::SpawnEliteEncounter()
{
	check(EliteEncounters);

	const FEliteEncounter& Encounter = EliteEncounters->Encounters[Rng->RandRange(0, EliteEncounters->Encounters.Num() - 1)];

	// Spawn the elite
	SpawnEnemy(Encounter.GetEliteClass(), Level);

	++LivingEnemies;

	// Spawn any additional enemies that are a part of the elite encounter
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
