// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/RoomManagers/HordeRoomManager.h"

#include "Idk/GameSystems/ConfirmationBox.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/RoomManagers/RoomManager.h"
#include "Idk/GameSystems/Spawners/HordeEnemySpawner.h"
#include <Curves/CurveFloat.h>
#include <Engine/EngineTypes.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

AHordeRoomManager::AHordeRoomManager()
{
	NextWaveConfirmationText = FText::FromString(TEXT("Spawn next wave?"));
}

#if WITH_EDITOR
EDataValidationResult AHordeRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (ConfirmationBoxClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AHordeRoomManager: Confirmation box class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AHordeRoomManager::Destroyed()
{
	if (EnemySpawner)
	{
		EnemySpawner->Destroy();
	}

	Super::Destroyed();
}

void AHordeRoomManager::SetSpawnerInfo(const USpawnableEnemyClasses& InEnemyClasses, const UCurveFloat& InSpawnBudgetCurve)
{
	EnemyClasses = &InEnemyClasses;
	SpawnBudgetCurve = &InSpawnBudgetCurve;
}

void AHordeRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (EnemySpawner)
	{
		EnemySpawner->Destroy();
	}
	
	if (ConfirmationBox)
	{
		ConfirmationBox->Destroy();
	}
}

void AHordeRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	EnemySpawner = GetWorld()->SpawnActor<AHordeEnemySpawner>();
	EnemySpawner->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	EnemySpawner->OnAllEnemiesKilledDelegate.BindUObject(this, &AHordeRoomManager::OnAllEnemiesKilled);
	EnemySpawner->OnWaveCompleteDelgate.BindUObject(this, &AHordeRoomManager::OnWaveComplete);

	EnemySpawner->Init(*Player, Level, Rng->GenerateSeed());
	EnemySpawner->SetSpawnerInfo(*EnemyClasses, *SpawnBudgetCurve);
	EnemySpawner->SetHordeSpawnerInfo(MaxWaves);

	EnemySpawner->SpawnWave();
}

void AHordeRoomManager::OnAllEnemiesKilled()
{
	if (EnemySpawner->AllWavesCompleted())
	{
		RewardLevel += CompletionRewardBonus;
	}

	Super::OnAllEnemiesKilled();
}

void AHordeRoomManager::OnWaveComplete()
{
	if (ConfirmationBox)
	{
		// Show the confirmation box
		ConfirmationBox->SetActorHiddenInGame(false);

		RewardLevel += PerWaveRewardBonus;
	}
	else
	{
		ConfirmationBox = GetWorld()->SpawnActor<AConfirmationBox>(ConfirmationBoxClass.LoadSynchronous());
		ConfirmationBox->GetRootComponent()->AttachToComponent(
			RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

		ConfirmationBox->SetMessage(NextWaveConfirmationText);
		ConfirmationBox->OnOptionSelectedDelegate.BindLambda([&](const bool bConfirmed)
			{
				if (bConfirmed)
				{
					// Hide the confirmation box
					ConfirmationBox->SetActorHiddenInGame(true);
					EnemySpawner->SpawnWave();
				}
				else
				{
					ConfirmationBox->Destroy();
					OnAllEnemiesKilled();
				}
			});
	}
}
