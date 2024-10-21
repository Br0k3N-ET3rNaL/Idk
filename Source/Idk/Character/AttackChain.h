// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "AttackChain.generated.h"

class UAnimMontage;

/** Struct that reperesents a chain of attack animations that can be used by a character. */
USTRUCT()
struct FAttackChain final
{
	GENERATED_BODY()

public:
	/** Get the current attack in the attack chain. */
	UE_NODISCARD const UAnimMontage* GetCurrentAttack() const;

	/** Switches to and gets the next attack in the attack chain. */
	UE_NODISCARD UAnimMontage& GetNextAttack();

	/** Sets the current attack to the first attack in the attack chain. */
	void ResetChain();

	/** Checks if the attack chain is valid. */
	UE_NODISCARD bool IsValid() const;

private:
	/** All attack animations in the attack chain. */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UAnimMontage>> AttackAnimations;

	/** Index of the current attack in the chain. */
	UPROPERTY(VisibleInstanceOnly)
	int32 CurrentAttackIndex = -1;

};

