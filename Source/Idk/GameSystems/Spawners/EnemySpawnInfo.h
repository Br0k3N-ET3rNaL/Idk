// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "EnemySpawnInfo.generated.h"

class AIdkEnemyCharacter;

/** Information used to spawn a group an enemies as part of an encounter. */
USTRUCT()
struct FEnemySpawnInfo final
{
	GENERATED_BODY()

public:
	/** Get the class of enemies to spawn. */
	UE_NODISCARD TSubclassOf<AIdkEnemyCharacter> GetEnemyClass() const;

	/** Get the number of enemies to spawn. */
	UE_NODISCARD uint8 GetNumToSpawn() const;

private:
	/** Blueprint class of the enemies to spawn. */
	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSoftClassPtr<AIdkEnemyCharacter> EnemyClass;

	/** Number of enemies to spawn. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1"))
	uint8 NumToSpawn = 1;
};

