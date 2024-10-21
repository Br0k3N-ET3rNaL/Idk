// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/RangedEnemyCharacter.h"
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "SiegeEnemy.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FObjectInitializer;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetPlantedDelegate, const bool, bPlanted);

/** Ranged enemy that roots itself in place to fire. */
UCLASS(Abstract, Blueprintable)
class ASiegeEnemy final : public ARangedEnemyCharacter
{
	GENERATED_BODY()

public:
	ASiegeEnemy(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Set whether the enemy is planted or not. Called by the enemy's behavior tree. */
	void SetPlanted(const bool bInPlanted);

	/** Delegate that fires when the animations for entering and exiting planted are finished. */
	FSimpleDelegate OnFinishedPlantingDelegate;

	/**
	 * Delegate that fires when entering or exiting the planted state.
	 * Informs the animation blueprint when the planted state changes.
	 * 
	 * @param Whether the enemy is planted or not.
	 */
	UPROPERTY(BlueprintAssignable)
	FSetPlantedDelegate SetPlantedDelegate;

private:
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin AIdkCharacter Interface.
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	//~ End AIdkCharacter Interface

	/** Animation that is played when entering the planted state. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UAnimMontage> EnterPlantedMontage;

	/** Animation that is played when exiting the planted state. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UAnimMontage> ExitPlantedMontage;

	/** Whether the enemy is planted or not. While planted, the enemy cannot move. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	bool bPlanted = false;

	/** Multiplied by the character's default rotation rate to get the rotation rate while planted. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ClampMin = 0.01, ClampMax = 1.0))
	double PlantedRotationMultiplier = 0.5;

	/** Default rotation rate. */
	FRotator BaseRotationRate;

	/** Rotation rate when planted. */
	FRotator PlantedRotationRate;
	
};
