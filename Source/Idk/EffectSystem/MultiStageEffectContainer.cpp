// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/EffectSystem/MultiStageEffectContainer.h"

#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/MultiStageBonusEffectContainer.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UnrealTemplate.h>

USingleStageEffect*& FMultiStageEffectContainer::operator[](const int32 Index)
{
	int32 ArrayIndex = -1;

	return GetStageArray(Index, ArrayIndex)[ArrayIndex];
}

const USingleStageEffect* const& FMultiStageEffectContainer::operator[](const int32 Index) const
{
	int32 ArrayIndex = -1;

	return GetStageArray(Index, ArrayIndex)[ArrayIndex];
}

void FMultiStageEffectContainer::AddToStage(USingleStageEffect& Effect, const EEffectStage Stage)
{
	check(Stage != EEffectStage::None);

	GetStageArray(Stage).Add(&Effect);
}

void FMultiStageEffectContainer::Append(const FMultiStageEffectContainer& Other)
{
	SelfEffects.Append(Other.SelfEffects);
	TargetEffects.Append(Other.TargetEffects);
	CallbackEffects.Append(Other.CallbackEffects);
}

void FMultiStageEffectContainer::Append(const FMultiStageBonusEffectContainer& BonusEffect, const bool bSelfEffectsOnly)
{
	SelfEffects.Append(BonusEffect.SelfEffects);

	if (!bSelfEffectsOnly)
	{
		TargetEffects.Append(BonusEffect.TargetEffects);
		CallbackEffects.Append(BonusEffect.CallbackEffects);
	}
}

bool FMultiStageEffectContainer::IsEmpty() const
{
	return Num() == 0;
}

bool FMultiStageEffectContainer::IsStageEmpty(const EEffectStage Stage) const
{
	check(Stage != EEffectStage::None);

	return GetStageArray(Stage).IsEmpty();
}

int32 FMultiStageEffectContainer::Num() const
{
	return SelfEffects.Num() + TargetEffects.Num() + CallbackEffects.Num();
}

bool FMultiStageEffectContainer::IsValidIndex(const int32 Index) const
{
	return Index >= 0 && Index < Num();
}

void FMultiStageEffectContainer::RemoveAt(const int32 Index)
{
	int32 ArrayIndex = -1;

	GetStageArray(Index, ArrayIndex).RemoveAt(ArrayIndex);
}

TIndexedContainerIterator<FMultiStageEffectContainer, USingleStageEffect*, int32> FMultiStageEffectContainer::CreateIterator()
{
	return TIndexedContainerIterator<FMultiStageEffectContainer, USingleStageEffect*, int32>(*this);
}

TIndexedContainerIterator<const FMultiStageEffectContainer, const USingleStageEffect* const, int32> FMultiStageEffectContainer::CreateConstIterator() const
{
	return TIndexedContainerIterator<const FMultiStageEffectContainer, const USingleStageEffect* const, int32>(*this);
}

TIndexedContainerIterator<TArray<USingleStageEffect*>, USingleStageEffect*, int32> FMultiStageEffectContainer::CreateStageIterator(const EEffectStage Stage)
{
	check(Stage != EEffectStage::None);

	return TIndexedContainerIterator<TArray<USingleStageEffect*>, USingleStageEffect*, int32>(GetStageArray(Stage));
}

TIndexedContainerIterator<const TArray<USingleStageEffect*>, const USingleStageEffect* const, int32> FMultiStageEffectContainer::CreateConstStageIterator(const EEffectStage Stage) const
{
	check(Stage != EEffectStage::None);

	return TIndexedContainerIterator<const TArray<USingleStageEffect*>, const USingleStageEffect* const, int32>(GetStageArray(Stage));
}

bool FMultiStageEffectContainer::IsValid(FString& OutInvalidReason) const
{
	if (SelfEffects.IsEmpty() && TargetEffects.IsEmpty() && CallbackEffects.IsEmpty())
	{
		OutInvalidReason = TEXT("All stages are empty, effect will have no effect");

		return false;
	}

	if (TargetEffects.IsEmpty() && !CallbackEffects.IsEmpty())
	{
		OutInvalidReason = TEXT("TargetEffects was empty but CallbackEffects was not, CallbackEffects can only contain effects if TargetEffects does");

		return false;
	}

	return true;
}

TArray<USingleStageEffect*>& FMultiStageEffectContainer::GetStageArray(const EEffectStage Stage)
{
	return const_cast<TArray<USingleStageEffect*>&>(AsConst(*this).GetStageArray(Stage));
}

const TArray<USingleStageEffect*>& FMultiStageEffectContainer::GetStageArray(const EEffectStage Stage) const
{
	switch (Stage)
	{
		case EEffectStage::Self:
			return SelfEffects;
		case EEffectStage::Target:
			return TargetEffects;
		case EEffectStage::Callback:
			return CallbackEffects;
		default:
			checkNoEntry();

			return SelfEffects;
			break;
	}
}

TArray<USingleStageEffect*>& FMultiStageEffectContainer::GetStageArray(const int32 InIndex, int32& OutIndex)
{
	return GetStageArray(GetStageAndIndex(InIndex, OutIndex));
}

const TArray<USingleStageEffect*>& FMultiStageEffectContainer::GetStageArray(const int32 InIndex, int32& OutIndex) const
{
	return GetStageArray(GetStageAndIndex(InIndex, OutIndex));
}

EEffectStage FMultiStageEffectContainer::GetStageAndIndex(const int32 InIndex, int32& OutIndex) const
{
	if (InIndex < SelfEffects.Num())
	{
		OutIndex = InIndex;

		return EEffectStage::Self;
	}

	int32 ModifiedIndex = InIndex - SelfEffects.Num();

	if (ModifiedIndex < TargetEffects.Num())
	{
		OutIndex = ModifiedIndex;

		return EEffectStage::Target;
	}

	ModifiedIndex -= TargetEffects.Num();

	if (ModifiedIndex < CallbackEffects.Num())
	{
		OutIndex = ModifiedIndex;

		return EEffectStage::Callback;
	}

	OutIndex = -1;

	return EEffectStage::None;
}
