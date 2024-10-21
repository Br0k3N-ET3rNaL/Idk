// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "MultiStageEffectContainer.generated.h"

enum class EEffectStage : uint8;
class FString;
class USingleStageEffect;
struct FMultiStageBonusEffectContainer;

/** Contains 3 stages of groups of single stage effects. */
USTRUCT()
struct FMultiStageEffectContainer final
{
	GENERATED_BODY()

public:
	UE_NODISCARD USingleStageEffect*& operator[](const int32 Index);
	UE_NODISCARD const USingleStageEffect* const& operator[](const int32 Index) const;

	/** Add the specified effect to the specified stage. */
	void AddToStage(USingleStageEffect& Effect, const EEffectStage Stage);

	/** Append another multistage effect container. */
	void Append(const FMultiStageEffectContainer& Other);

	/**
	 * Append a multistage bonus effect container.
	 * 
	 * @param BonusEffect		Multistage bonus effect container.
	 * @param bSelfEffectsOnly	Whether to only append self effects or all 3 stages.
	 */
	void Append(const FMultiStageBonusEffectContainer& BonusEffect, const bool bSelfEffectsOnly);

	/** Check whether all stages are empty. */
	UE_NODISCARD bool IsEmpty() const;

	/** Check whether the specified stage is empty. */
	UE_NODISCARD bool IsStageEmpty(const EEffectStage Stage) const;

	/** Get the number of effects in the container. */
	UE_NODISCARD int32 Num() const;

	/** Check if the specified index is within the range of the container. */
	UE_NODISCARD bool IsValidIndex(const int32 Index) const;

	/** Remove the effect at the specified index. */
	void RemoveAt(const int32 Index);

	/** Create an iterator over all effects in the container. */
	UE_NODISCARD TIndexedContainerIterator<FMultiStageEffectContainer, USingleStageEffect*, int32> CreateIterator();

	/** Create a const iterator over all effects in the container. */
	UE_NODISCARD TIndexedContainerIterator<const FMultiStageEffectContainer, const USingleStageEffect* const, int32> CreateConstIterator() const;

	/** Create an iterator over all effects in the specified stage. */
	UE_NODISCARD TIndexedContainerIterator<TArray<USingleStageEffect*>, USingleStageEffect*, int32> CreateStageIterator(const EEffectStage Stage);

	/** Create a const iterator over all effects in the specified stage. */
	UE_NODISCARD TIndexedContainerIterator<const TArray<USingleStageEffect*>, const USingleStageEffect* const, int32> CreateConstStageIterator(const EEffectStage Stage) const;

	/**
	 * Check if the container is valid.
	 * 
	 * @param OutInvalidReason	The reason the container is invald. 
	 * @return					True if the container is valid, otherwise false.
	 */
	UE_NODISCARD bool IsValid(FString& OutInvalidReason) const;

private:
	/** Get the array for the specified stage. */
	TArray<USingleStageEffect*>& GetStageArray(const EEffectStage Stage);

	/** Get the array for the specified stage. */
	UE_NODISCARD const TArray<USingleStageEffect*>& GetStageArray(const EEffectStage Stage) const;

	/**
	 * Get the array for the stage that the index is in.
	 * 
	 * @param InIndex	Index within the combination of all 3 stages.
	 * @param OutIndex	Index within the specific stage.
	 * @return			Array for the stage that the index is in.
	 */
	UE_NODISCARD TArray<USingleStageEffect*>& GetStageArray(const int32 InIndex, int32& OutIndex);

	/**
	 * Get the array for the stage that the index is in.
	 *
	 * @param InIndex	Index within the combination of all 3 stages.
	 * @param OutIndex	Index within the specific stage.
	 * @return			Array for the stage that the index is in.
	 */
	UE_NODISCARD const TArray<USingleStageEffect*>& GetStageArray(const int32 InIndex, int32& OutIndex) const;

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
	TArray<USingleStageEffect*> SelfEffects;

	/** Effects that will be applied to the target. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<USingleStageEffect*> TargetEffects;

	/** Effects that will be applied to the one who applies the multistage effect, after applying effects to the target. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<USingleStageEffect*> CallbackEffects;

};
