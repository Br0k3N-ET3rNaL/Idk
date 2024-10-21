// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/Spawners/EnemySpawnInfo.h"
#include <Containers/Array.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "BossEncounters.generated.h"

class ABossEnemyCharacter;
class AIdkEnemyCharacter;

/** Information needed to spawn a boss encounter. */
USTRUCT()
struct FBossEncounter final
{
	GENERATED_BODY()

public:
	/** Get the class of the boss. */
	UE_NODISCARD TSubclassOf<ABossEnemyCharacter> GetBossClass() const;

	/** Get the information needed to spawn any additional enemies. */
	UE_NODISCARD const TArray<FEnemySpawnInfo>& GetAdds() const;

private:
	/** Class of the boss to spawn. */
	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSoftClassPtr<ABossEnemyCharacter> BossClass;

	/** Information needed to spawn any additional enemies. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FEnemySpawnInfo> Adds;
};

/** Contains all possible boss encounters that can be spawned. */
UCLASS()
class UBossEncounters final : public UDataAsset
{
	GENERATED_BODY()

public:
	/** All possible boss encounters. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FBossEncounter> Encounters;

};
