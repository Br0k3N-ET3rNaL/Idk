// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/CombatRoomManager.h"

#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include "Idk/GameSystems/Spawners/BasicEnemySpawner.h"
#include <Curves/CurveFloat.h>
#include <Engine/EngineTypes.h>

void ACombatRoomManager::Destroyed()
{
	if (EnemySpawner)
	{
		EnemySpawner->Destroy();
	}

	Super::Destroyed();
}

void ACombatRoomManager::SetSpawnerInfo(const USpawnableEnemyClasses& InEnemyClasses, const UCurveFloat& InSpawnBudgetCurve)
{
	EnemyClasses = &InEnemyClasses;
	SpawnBudgetCurve = &InSpawnBudgetCurve;
}

void ACombatRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (EnemySpawner)
	{
		EnemySpawner->Destroy();
	}
}

void ACombatRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	EnemySpawner = GetWorld()->SpawnActor<ABasicEnemySpawner>();
	EnemySpawner->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	EnemySpawner->OnAllEnemiesKilledDelegate.BindUObject(this, &ACombatRoomManager::OnAllEnemiesKilled);

	EnemySpawner->Init(*Player, Level, Rng->GenerateSeed());
	EnemySpawner->SetSpawnerInfo(*EnemyClasses, *SpawnBudgetCurve);

	EnemySpawner->SpawnEnemies();
}
