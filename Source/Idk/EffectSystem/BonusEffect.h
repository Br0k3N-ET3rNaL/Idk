// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SingleStageEffectGroup.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BonusEffect.generated.h"

class FName;
class UObject;
struct FApplyEffectParams;

/** Effect that can be added to a stage in a multistage effect. */
UCLASS(EditInlineNew)
class UBonusEffect final : public USingleStageEffectGroup
{
	GENERATED_BODY()

	friend class FBonusEffectBuilder;
	
public:
	//~ Begin UEffect Interface.
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	//~ Begin USingleStageEffect Interface.
	virtual void ApplyEffect(FApplyEffectParams& Params) const override final;
	//~ End USingleStageEffect Interface

	/** Set the bonus effect's ID. */
	void SetId(const FEffectId InId);

	/** Get the bonus effect's current ID. */
	UE_NODISCARD FEffectId GetId() const;

	/** Check whether the bonus effect's ID is equal to the specified ID. */
	UE_NODISCARD bool IsIdEqualTo(const FEffectId InId) const;

	/** Increment the bonus effect's stacks. */
	void IncrementStacks();

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/** ID for the bonus effect. */
	UPROPERTY(VisibleInstanceOnly, meta = (ShowOnlyInnerProperties))
	FEffectId Id = FEffectId::NoId;

	/** Number of stacks of the bonus effect that are currently applied. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "Stacks > 1", EditConditionHides))
	uint8 Stacks = 1;

};
