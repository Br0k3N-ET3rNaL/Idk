// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <Containers/Array.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "EliteEncounters.generated.h"

class AEliteEnemyCharacter;
class AIdkEnemyCharacter;

/** Information needed to spawn an elite encounter */
USTRUCT()
struct FEliteEncounter final
{
	GENERATED_BODY()

public:
	/** Get the class of the elite to spawn. */
	UE_NODISCARD TSubclassOf<AEliteEnemyCharacter> GetEliteClass() const;

	/** Get the information needed to spawn any additional enemies. */
	UE_NODISCARD const TArray<FEnemySpawnInfo>& GetAdds() const;

private:
	/** Class of the elite to spawn. */
	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSoftClassPtr<AEliteEnemyCharacter> EliteClass;

	/** Information needed to spawn any additional enemies. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FEnemySpawnInfo> Adds;
};

/** Contains all possible elite encounters that can be spawned. */
UCLASS()
class UEliteEncounters final : public UDataAsset
{
	GENERATED_BODY()

public:
	/** All possible elite encounters that can be spawned. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FEliteEncounter> Encounters;
	
};
