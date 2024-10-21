// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/BossRoomManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Spawners/BossEnemySpawner.h"
#include <Engine/EngineTypes.h>

void ABossRoomManager::Destroyed()
{
	if (Spawner)
	{
		Spawner->Destroy();
	}

	Super::Destroyed();
}

void ABossRoomManager::SetEncounters(const UBossEncounters& Encounters)
{
	BossEncounters = &Encounters;
}

void ABossRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (Spawner)
	{
		Spawner->Destroy();
	}
}

void ABossRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	Spawner = GetWorld()->SpawnActor<ABossEnemySpawner>();
	Spawner->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Spawner->OnAllEnemiesKilledDelegate.BindUObject(this, &ABossRoomManager::OnAllEnemiesKilled);

	Spawner->Init(*Player, Level, Rng->GenerateSeed());
	Spawner->SetEncounters(*BossEncounters);

	Spawner->SpawnBoss();
}

void ABossRoomManager::OnAllEnemiesKilled()
{
	Player->OnBossKilled();
}
