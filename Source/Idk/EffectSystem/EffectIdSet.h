// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectId.h"
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "EffectIdSet.generated.h"

/** A set of effect IDs. */
USTRUCT()
struct FEffectIdSet final
{
	GENERATED_BODY()

public:
	FEffectIdSet() = default;
	FEffectIdSet(const TSet<FEffectId>& IdSet);

	/** Get the next valid effect ID. */
	UE_NODISCARD FEffectId GetNextValidId() const;

	/** Check if the ID set is empty. */
	UE_NODISCARD bool IsEmpty() const;

	/** Get the number of effect IDs in the set. */
	UE_NODISCARD int32 Num() const;

	/** Add an effect ID to the set. */
	void Add(FEffectId Id);

	/** Remove an effect ID from the set. */
	void Remove(FEffectId Id);

	/** Get the intersection of the ID set with another ID set. */
	UE_NODISCARD TSet<FEffectId> Intersect(const FEffectIdSet& Other) const;

	/** Append another ID set. */
	void Append(const FEffectIdSet& Other);

	/** Check if the ID set contains the specified effect ID. */
	UE_NODISCARD bool Contains(const FEffectId Id) const;

	/** Get the first ID in the set. */
	UE_NODISCARD FEffectId GetFirstId() const;

	/**
	 * Get the next valid effect ID.
	 * 
	 * @param ExistingIds	Set of existing effect IDs. 
	 * @param MinValidId	Minimum valid effect ID.
	 * @return				The next valid effect ID. 
	 */
	UE_NODISCARD static FEffectId GetNextValidId(const TSet<FEffectId>& ExistingIds, const FEffectId MinValidId = FEffectId::MinValidId);

	/**
	 * Get multiple valid effect IDs.
	 * 
	 * @param ExistingIds		Set of existing effect IDs.  
	 * @param NumIdsToGenerate	Number of valid effect IDs to get.
	 * @return					Set containing the valid effect IDs.
	 */
	UE_NODISCARD static TSet<FEffectId> GetValidIds(const TSet<FEffectId>& ExistingIds, const int32 NumIdsToGenerate);

	/**
	 * Maps old effect IDs to new effect IDs.
	 * 
	 * @param OldIds	Set containing old effect IDs. 
	 * @param NewIds	Set containing new effect IDs.
	 * @return			Maps old effect IDs to new effect IDs. 
	 */
	UE_NODISCARD static TMap<FEffectId, FEffectId> GetIdSwapMap(const TSet<FEffectId>& OldIds, const TSet<FEffectId>& NewIds);

private:
	/** Update the minimum valid effect ID based on IDs in the set. */
	void UpdateMinValidId();

	/** Set of effect IDs. */
	UPROPERTY(VisibleAnywhere, meta = (ShowOnlyInnerProperties))
	TSet<FEffectId> IdSet;

	/** Minimum effect ID that isn't in the set. */
	FEffectId MinValidId = FEffectId::MinValidId;
};
