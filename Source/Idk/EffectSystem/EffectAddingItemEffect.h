// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "EffectAddingItemEffect.generated.h"

enum class EAddEffectLocation : uint8;
enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class UAttributeSystemComponent;
class UBonusEffect;
class UMultiStageBonusEffect;
class UObject;

/** Item effect that adds a bonus effect to a specified location on the target. */
UCLASS()
class UEffectAddingItemEffect final : public UItemEffect
{
	GENERATED_BODY()

	friend class FEffectAddingItemEffectBuilder;

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
	/** Where to add the bonus effec to. */
	UPROPERTY(EditAnywhere)
	EAddEffectLocation LocationToAddEffect;

	/** The bonus effect to add. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TObjectPtr<UMultiStageBonusEffect> Effect;

	/** ID of the added bonus effect. */
	UPROPERTY(VisibleInstanceOnly, meta = (ShowOnlyInnerProperties))
	mutable FEffectId BonusEffectId = FEffectId::NoId;

};
