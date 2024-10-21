// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/EnemySpawner.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include <Components/SceneComponent.h>
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>
#include <UObject/Object.h>

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AEnemySpawner::Destroyed()
{
	while (!Enemies.IsEmpty())
	{
		Enemies.Pop()->Destroy();
	}

	Super::Destroyed();
}

void AEnemySpawner::Init(AIdkPlayerCharacter& InPlayer, const uint8 InLevel, const int32 InSeed)
{
	Player = &InPlayer;
	Level = InLevel;
	Seed = InSeed;
	SeedString = UIdkRandomStream::SeedToString(Seed);

	Rng = &UIdkRandomStream::CreateRandomStream(this, Seed);
}

void AEnemySpawner::OnEnemyDeath(const int32 Experience, const int32 Gold)
{
	if (IsValid(Player))
	{
		Player->AddExperience(Experience);
		Player->AddGold(Gold);
	}

	if (--LivingEnemies == 0)
	{
		OnAllEnemiesKilled();
	}
}

void AEnemySpawner::SpawnEnemy(TSubclassOf<AIdkEnemyCharacter> EnemyClass, const uint8 EnemyLevel, const float SpawnDelay)
{
	if (SpawnDelay <= 0.f)
	{
		SpawnEnemyInternal(EnemyClass, EnemyLevel);
	}
	else
	{
		FTimerHandle Timer;

		// Spawn the enemy after a delay
		GetWorldTimerManager().SetTimer(Timer, [=, this]()
			{
				SpawnEnemyInternal(EnemyClass, EnemyLevel);
			}, 
			SpawnDelay, false);
	}
}

void AEnemySpawner::OnAllEnemiesKilled()
{
	OnAllEnemiesKilledDelegate.ExecuteIfBound();
}

void AEnemySpawner::SpawnEnemyInternal(TSubclassOf<AIdkEnemyCharacter> EnemyClass, const uint8 EnemyLevel)
{
	const double SpawnZOffset = CastChecked<AIdkEnemyCharacter>(EnemyClass->GetDefaultObject())->GetSpawnZOffset();
	const FVector SpawnOffset = FVector(0.0, 0.0, SpawnZOffset);

	const FTransform SpawnTransform = FTransform(GetActorQuat(), GetActorLocation() + SpawnOffset);

	// Spawn the enemy
	AIdkEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AIdkEnemyCharacter>(EnemyClass
		, SpawnTransform, this, nullptr
		, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	// Initialize the enemy
	Enemy->Init(EnemyLevel);
	Enemy->OnDeathDelegate.BindUObject(this, &AEnemySpawner::OnEnemyDeath);

	Enemies.Add(Enemy);

	UGameplayStatics::FinishSpawningActor(Enemy, SpawnTransform);
}
