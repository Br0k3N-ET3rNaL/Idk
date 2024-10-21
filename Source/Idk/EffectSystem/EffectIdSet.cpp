// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectIdSet.h"

#include "Idk/EffectSystem/EffectId.h"
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>

FEffectIdSet::FEffectIdSet(const TSet<FEffectId>& IdSet)
	: IdSet(IdSet)
{
	UpdateMinValidId();
}

FEffectId FEffectIdSet::GetNextValidId() const
{
	return GetNextValidId(IdSet, MinValidId);
}

bool FEffectIdSet::IsEmpty() const
{
	return IdSet.IsEmpty();
}

int32 FEffectIdSet::Num() const
{
	return IdSet.Num();
}

void FEffectIdSet::Add(FEffectId Id)
{
	if (MinValidId == Id)
	{
		// Update the minimum valid effect ID
		++MinValidId;
	}

	IdSet.Add(Id);
}

void FEffectIdSet::Remove(FEffectId Id)
{
	// Update the minimum valid effect ID
	MinValidId = (Id < MinValidId) ? Id : MinValidId;

	IdSet.Remove(Id);
}

TSet<FEffectId> FEffectIdSet::Intersect(const FEffectIdSet& Other) const
{
	return IdSet.Intersect(Other.IdSet);
}

void FEffectIdSet::Append(const FEffectIdSet& Other)
{
	IdSet.Append(Other.IdSet);

	UpdateMinValidId();
}

bool FEffectIdSet::Contains(const FEffectId Id) const
{
	return IdSet.Contains(Id);
}

FEffectId FEffectIdSet::GetFirstId() const
{
	for (const FEffectId Id : IdSet)
	{
		return Id;
	}

	return FEffectId::NoId;
}

FEffectId FEffectIdSet::GetNextValidId(const TSet<FEffectId>& ExistingIds, const FEffectId MinValidId)
{
	FEffectId ValidId = MinValidId;

	while (ExistingIds.Contains(ValidId))
	{
		++ValidId;
	}

	return ValidId;
}

TSet<FEffectId> FEffectIdSet::GetValidIds(const TSet<FEffectId>& ExistingIds, const int32 NumIdsToGenerate)
{
	FEffectIdSet IdSet = FEffectIdSet(ExistingIds);

	for (int32 i = 0; i < NumIdsToGenerate; ++i)
	{
		IdSet.Add(IdSet.GetNextValidId());
	}

	return IdSet.IdSet;
}

TMap<FEffectId, FEffectId> FEffectIdSet::GetIdSwapMap(const TSet<FEffectId>& OldIds, const TSet<FEffectId>& NewIds)
{
	check(OldIds.Num() == NewIds.Num());

	TMap<FEffectId, FEffectId> IdMap;

	auto OldIdsIt = OldIds.CreateConstIterator();
	auto NewIdsIt = NewIds.CreateConstIterator();

	while (OldIdsIt && NewIdsIt)
	{
		IdMap.Add(*OldIdsIt, *NewIdsIt);

		++OldIdsIt;
		++NewIdsIt;
	}

	return IdMap;
}

void FEffectIdSet::UpdateMinValidId()
{
	MinValidId = GetNextValidId();
}
