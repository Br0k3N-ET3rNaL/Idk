// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/Components/AbilityComponent.h"
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AIAbilityComponent.generated.h"

class AActor;
class AIdkCharacter;
class AIdkEnemyCharacter;
class AIdkPlayerCharacter;

DECLARE_DELEGATE_OneParam(FSetOnCooldownDelegate, const bool);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UAIAbilityComponent final : public UAbilityComponent
{
	GENERATED_BODY()

public:
	//~ Begin UAbilityComponent public Interface.
	virtual void EndAbility() override;
	//~End UAbilityComponent public Interface

	/** Set the target of the ability. */
	void SetTarget(const AActor* InTarget);

	/** Get a position in range of the target, */
	UE_NODISCARD FVector GetPosInRangeOfTarget(const AActor* InTarget) const;

	/** Check if the component is in range of the target. */
	UE_NODISCARD bool IsInRangeOfTarget(const AActor* InTarget) const;

	/**
	 * Delegate called when ability goes on or off cooldown.
	 * 
	 * @param bOnCooldown Whether the ability is on cooldown.
	 */
	FSetOnCooldownDelegate SetOnCooldownDelegate;

	/** Delegate called once the ability ends. */
	FSimpleDelegate OnAbilityEndDelegate;

private:
	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	//~ Begin UAbilityComponent protected Interface.
	UE_NODISCARD virtual FVector GetRangedTargetLocation() const override final;
	UE_NODISCARD virtual TSubclassOf<AIdkCharacter> GetCollisionClassFilter() const override final;
	UE_NODISCARD virtual FVector GetAimLocation() const override final;
	//~ End UAbilityComponent protected Interface

	/** Event called when ability goes off cooldown. */
	void OnCooldownEnd();

	/** Owning enemy. */
	TObjectPtr<AIdkEnemyCharacter> Owner;

	/** Timer for cooldown. */
	FTimerHandle CooldownTimer;

	/** Target of the ability. */
	TObjectPtr<const AActor> Target;
};
