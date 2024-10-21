// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/EliteRoomManager.h"

#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include "Idk/GameSystems/Spawners/BasicEnemySpawner.h"
#include "Idk/GameSystems/Spawners/EliteEnemySpawner.h"
#include <Engine/EngineTypes.h>

void AEliteRoomManager::Destroyed()
{
	if (Spawner)
	{
		Spawner->Destroy();
	}

	Super::Destroyed();
}

void AEliteRoomManager::SetEncounters(const UEliteEncounters& Encounters)
{
	EliteEncounters = &Encounters;
}

void AEliteRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (Spawner)
	{
		Spawner->Destroy();
	}
}

void AEliteRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	Spawner = GetWorld()->SpawnActor<AEliteEnemySpawner>();
	Spawner->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Spawner->OnAllEnemiesKilledDelegate.BindUObject(this, &AEliteRoomManager::OnAllEnemiesKilled);

	Spawner->Init(*Player, Level, Rng->GenerateSeed());
	Spawner->SetEncounters(*EliteEncounters);
	Spawner->SpawnEliteEncounter();
}
