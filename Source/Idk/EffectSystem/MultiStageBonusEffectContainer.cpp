// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/EffectSystem/MultiStageBonusEffectContainer.h"

#include "Idk/EffectSystem/EffectStage.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UnrealTemplate.h>

UBonusEffect*& FMultiStageBonusEffectContainer::operator[](const int32 Index)
{
	int32 ArrayIndex = -1;

	return GetStageArray(Index, ArrayIndex)[ArrayIndex];
}

const UBonusEffect* const& FMultiStageBonusEffectContainer::operator[](const int32 Index) const
{
	int32 ArrayIndex = -1;

	return GetStageArray(Index, ArrayIndex)[ArrayIndex];
}

void FMultiStageBonusEffectContainer::AddToStage(UBonusEffect& Effect, const EEffectStage Stage)
{
	GetStageArray(Stage).Add(&Effect);
}

void FMultiStageBonusEffectContainer::Append(const FMultiStageBonusEffectContainer& Other)
{
	SelfEffects.Append(Other.SelfEffects);
	TargetEffects.Append(Other.TargetEffects);
	CallbackEffects.Append(Other.CallbackEffects);
}

bool FMultiStageBonusEffectContainer::IsEmpty() const
{
	return Num() == 0;
}

bool FMultiStageBonusEffectContainer::IsStageEmpty(const EEffectStage Stage) const
{
	return GetStageArray(Stage).IsEmpty();
}

int32 FMultiStageBonusEffectContainer::Num() const
{
	return SelfEffects.Num() + TargetEffects.Num() + CallbackEffects.Num();
}

bool FMultiStageBonusEffectContainer::IsValidIndex(const int32 Index) const
{
	return Index >= 0 && Index < Num();
}

void FMultiStageBonusEffectContainer::RemoveAt(const int32 Index)
{
	int32 ArrayIndex = -1;

	GetStageArray(Index, ArrayIndex).RemoveAt(ArrayIndex);
}

TIndexedContainerIterator<FMultiStageBonusEffectContainer, UBonusEffect*, int32> FMultiStageBonusEffectContainer::CreateIterator()
{
	return TIndexedContainerIterator<FMultiStageBonusEffectContainer, UBonusEffect*, int32>(*this);
}

TIndexedContainerIterator<const FMultiStageBonusEffectContainer, const UBonusEffect* const, int32> FMultiStageBonusEffectContainer::CreateConstIterator() const
{
	return TIndexedContainerIterator<const FMultiStageBonusEffectContainer, const UBonusEffect* const, int32>(*this);
}

TIndexedContainerIterator<TArray<UBonusEffect*>, UBonusEffect*, int32> FMultiStageBonusEffectContainer::CreateStageIterator(const EEffectStage Stage)
{
	return TIndexedContainerIterator<TArray<UBonusEffect*>, UBonusEffect*, int32>(GetStageArray(Stage));
}

TIndexedContainerIterator<const TArray<UBonusEffect*>, const UBonusEffect* const, int32> FMultiStageBonusEffectContainer::CreateConstStageIterator(const EEffectStage Stage) const
{
	return TIndexedContainerIterator<const TArray<UBonusEffect*>, const UBonusEffect* const, int32>(GetStageArray(Stage));
}

bool FMultiStageBonusEffectContainer::IsValid(FString& OutInvalidReason) const
{
	if (SelfEffects.IsEmpty() && TargetEffects.IsEmpty() && CallbackEffects.IsEmpty())
	{
		OutInvalidReason = TEXT("All stages are empty, effect will have no effect");

		return false;
	}

	return true;
}

TArray<UBonusEffect*>& FMultiStageBonusEffectContainer::GetStageArray(const EEffectStage Stage)
{
	return const_cast<TArray<UBonusEffect*>&>(AsConst(*this).GetStageArray(Stage));
}

const TArray<UBonusEffect*>& FMultiStageBonusEffectContainer::GetStageArray(const EEffectStage Stage) const
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

TArray<UBonusEffect*>& FMultiStageBonusEffectContainer::GetStageArray(const int32 InIndex, int32& OutIndex)
{
	return GetStageArray(GetStageAndIndex(InIndex, OutIndex));
}

const TArray<UBonusEffect*>& FMultiStageBonusEffectContainer::GetStageArray(const int32 InIndex, int32& OutIndex) const
{
	return GetStageArray(GetStageAndIndex(InIndex, OutIndex));
}

EEffectStage FMultiStageBonusEffectContainer::GetStageAndIndex(const int32 InIndex, int32& OutIndex) const
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
