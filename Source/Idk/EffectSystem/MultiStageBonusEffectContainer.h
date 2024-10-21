// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "MultiStageBonusEffectContainer.generated.h"

enum class EEffectStage : uint8;
class FString;
class UBonusEffect;

/** Contains 3 stages of groups of bonus effects. */
USTRUCT()
struct FMultiStageBonusEffectContainer final
{
	GENERATED_BODY()

	friend class FMultiStageBonusEffectBuilder;
	friend struct FMultiStageEffectContainer;

public:
	UE_NODISCARD UBonusEffect*& operator[](const int32 Index);
	UE_NODISCARD const UBonusEffect* const& operator[](const int32 Index) const;

	/** Add the specified effect to the specified stage. */
	void AddToStage(UBonusEffect& Effect, const EEffectStage Stage);

	/** Append another multistage bonus effect container. */
	void Append(const FMultiStageBonusEffectContainer& Other);

	/** Check whether all stages are empty. */
	UE_NODISCARD bool IsEmpty() const;

	/** Check whether the specified stage is empty. */
	UE_NODISCARD bool IsStageEmpty(const EEffectStage Stage) const;

	/** Get the number of bonus effects in the container. */
	UE_NODISCARD int32 Num() const;

	/** Check if the specified index is within the range of the container. */
	UE_NODISCARD bool IsValidIndex(const int32 Index) const;

	/** Remove the effect at the specified index. */
	void RemoveAt(const int32 Index);

	/** Create an iterator over all bonus effects in the container. */
	UE_NODISCARD TIndexedContainerIterator<FMultiStageBonusEffectContainer, UBonusEffect*, int32> CreateIterator();

	/** Create a const iterator over all bonus effects in the container. */
	UE_NODISCARD TIndexedContainerIterator<const FMultiStageBonusEffectContainer, const UBonusEffect* const, int32> CreateConstIterator() const;

	/** Create an iterator over all bonus effects in the specified stage. */
	UE_NODISCARD TIndexedContainerIterator<TArray<UBonusEffect*>, UBonusEffect*, int32> CreateStageIterator(const EEffectStage Stage);

	/** Create a const iterator over all bonus effects in the specified stage. */
	UE_NODISCARD TIndexedContainerIterator<const TArray<UBonusEffect*>, const UBonusEffect* const, int32> CreateConstStageIterator(const EEffectStage Stage) const;
	
	/**
	 * Check if the container is valid.
	 *
	 * @param OutInvalidReason	The reason the container is invald.
	 * @return					True if the container is valid, otherwise false.
	 */
	UE_NODISCARD bool IsValid(FString& OutInvalidReason) const;

private:
	/** Get the array for the specified stage. */
	UE_NODISCARD TArray<UBonusEffect*>& GetStageArray(const EEffectStage Stage);

	/** Get the array for the specified stage. */
	UE_NODISCARD const TArray<UBonusEffect*>& GetStageArray(const EEffectStage Stage) const;

	/**
	 * Get the array for the stage that the index is in.
	 *
	 * @param InIndex	Index within the combination of all 3 stages.
	 * @param OutIndex	Index within the specific stage.
	 * @return			Array for the stage that the index is in.
	 */
	UE_NODISCARD TArray<UBonusEffect*>& GetStageArray(const int32 InIndex, int32& OutIndex);

	/**
	 * Get the array for the stage that the index is in.
	 *
	 * @param InIndex	Index within the combination of all 3 stages.
	 * @param OutIndex	Index within the specific stage.
	 * @return			Array for the stage that the index is in.
	 */
	UE_NODISCARD const TArray<UBonusEffect*>& GetStageArray(const int32 InIndex, int32& OutIndex) const;


	/**
	 * Get the stage and index for that stage based on an index within the combination of all 3 stages.
	 *
	 * @param InIndex	Index within the combination of all 3 stages.
	 * @param OutIndex	Index within the specific stage.
	 * @return			Stage that the index is in.
	 */
	UE_NODISCARD EEffectStage GetStageAndIndex(const int32 InIndex, int32& OutIndex) const;

	/** Effects that will be applied to the one who applies the multistage effect. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<UBonusEffect*> SelfEffects;

	/** Effects that will be applied to the target. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<UBonusEffect*> TargetEffects;

	/** Effects that will be applied to the one who applies the multistage effect, after applying effects to the target. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<UBonusEffect*> CallbackEffects;

};

