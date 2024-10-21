// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include "Idk/EffectSystem/PartialEffectInfo.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "PartialEffectAddingItemEffect.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class UObject;

/** Item effect that adds a partial effect (e.g. lifesteal) to an effect on the target. */
UCLASS()
class UPartialEffectAddingItemEffect final : public UItemEffect
{
	GENERATED_BODY()

	friend class FPartialEffectAddingEffectBuilder;
	
public:
	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	//~ Begin UItemEffect Interface.
	virtual void ApplyEffect(UAttributeSystemComponent* Target) const override;
	virtual void RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks = 1) const override;
	//~ End UItemEffect Interface

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/** Information representing the partial effect. @see FPartialEffectInfo */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FPartialEffectInfo Info;

	/** ID of the added partial effect. */
	UPROPERTY(VisibleInstanceOnly)
	mutable FEffectId Id = FEffectId::NoId;

};
