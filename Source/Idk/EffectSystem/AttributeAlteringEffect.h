// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UnrealType.h>

#include "AttributeAlteringEffect.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class UAttributeSystemComponent;
class UObject;

/** Item effect that modifies a target's attributes. */
UCLASS()
class UAttributeAlteringEffect final : public UItemEffect
{
	GENERATED_BODY()

	friend class FAttributeAlteringEffectBuilder;

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

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
	/** Attribute to alter. */
	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FAttribute Attribute = FAttribute(true);

};
