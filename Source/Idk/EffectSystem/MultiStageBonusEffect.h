// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectIdSet.h"
#include "Idk/EffectSystem/MultiStageBonusEffectContainer.h"
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "MultiStageBonusEffect.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class UObject;

/** Effect containing bonus effects that can be added to a multistage effect. */
UCLASS()
class UMultiStageBonusEffect final : public UEffect
{
	GENERATED_BODY()

	friend class UMultiStageEffect;
	friend class FMultiStageBonusEffectBuilder;
	
public:
	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	/** Set the bonus effect ID for all bonus effects */
	void SetBonusEffectId(const FEffectId Id);
	
	/**
	 * Add a multistage bonus effect.
	 * 
	 * @param Other			Bonus effect to add. 
	 * @param InOutEffectId ID of the multistage bonus effect.
	 * @return				False if the ID was already set, otherwise true.
	 */
	bool AddBonusEffect(const UMultiStageBonusEffect& Other, FEffectId& InOutEffectId);

	/** Remove all bonus effects with the specified ID. */
	void RemoveBonusEffects(const FEffectId BonusEffectId);

	/** Get the number of bonus effects. */
	UE_NODISCARD int32 GetNumBonusEffects() const;

	/** Whether the multistage bonus effect has any subeffects. */
	UE_NODISCARD bool HasEffects() const;

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/** Get all IDs saved by subeffects. */
	UE_NODISCARD TSet<FEffectId> GetSaveIds() const;

	/**
	 * Swaps save and use IDs from subeffects.
	 * 
	 * @param OldIds	Previous IDs. 
	 * @param NewIds	New IDs.
	 */
	void SwapEffectIds(const TSet<FEffectId>& OldIds, const TSet<FEffectId>& NewIds);

	/** Contains all bonus effects. */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FMultiStageBonusEffectContainer Effects;

	/** Set of IDs used by bonus effects. */
	UPROPERTY()
	FEffectIdSet BonusEffectIds;

};
