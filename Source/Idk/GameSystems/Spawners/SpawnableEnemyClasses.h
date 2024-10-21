// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Array.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

#include "SpawnableEnemyClasses.generated.h"

class AIdkEnemyCharacter;
struct FPropertyChangedEvent;

UENUM()
enum class EEnemyRole
{
	None UMETA(Hidden),
	Melee,
	Ranged,
	Support,
};

/** Information representing an enemy type that can be spawned. */
USTRUCT()
struct FEnemyTypeInfo final
{
	GENERATED_BODY()

public:
	UE_NODISCARD bool operator<(const FEnemyTypeInfo& Other) const;

	/** Get the cost of spawning the enemy. */
	UE_NODISCARD int32 GetSpawnCost() const;

	/** Get the role of the enemy. */
	UE_NODISCARD EEnemyRole GetEnemyRole() const;

	/** Get the class of the enemy. */
	UE_NODISCARD TSubclassOf<AIdkEnemyCharacter> GetEnemyClass() const;

private:
	/** Cost of spawning the enemy. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1"))
	int32 SpawnCost = 1;

	/** Role of the enemy. */
	UPROPERTY(EditDefaultsOnly)
	EEnemyRole EnemyType = EEnemyRole::None;

	/** Class of the enemy. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AIdkEnemyCharacter> EnemyClass;

};

/** Contains information about all spawnable enemy types. */
UCLASS()
class USpawnableEnemyClasses final : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif 
	//~ End UObject Interface

	/** Information about all spawnable enemy types. */
	UPROPERTY(EditDefaultsOnly, meta = (ShowOnlyInnerProperties))
	TArray<FEnemyTypeInfo> EnemyTypes;
};
