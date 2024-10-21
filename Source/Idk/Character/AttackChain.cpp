// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/AttackChain.h"

#include <Animation/AnimMontage.h>
#include <UObject/SoftObjectPtr.h>

const UAnimMontage* FAttackChain::GetCurrentAttack() const
{
	return (CurrentAttackIndex >= 0) ? AttackAnimations[CurrentAttackIndex].LoadSynchronous() : nullptr;
}

UAnimMontage& FAttackChain::GetNextAttack()
{
	CurrentAttackIndex = (CurrentAttackIndex + 1) % AttackAnimations.Num();

	return *AttackAnimations[CurrentAttackIndex].LoadSynchronous();
}

void FAttackChain::ResetChain()
{
	CurrentAttackIndex = -1;
}

bool FAttackChain::IsValid() const
{
	if (AttackAnimations.IsEmpty())
	{
		return false;
	}
	else
	{
		for (const TSoftObjectPtr<UAnimMontage>& AttackAnimation : AttackAnimations)
		{
			if (AttackAnimation.IsNull())
			{
				return false;
			}
		}
	}

	return true;
}
