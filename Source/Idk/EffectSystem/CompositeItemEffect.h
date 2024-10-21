// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "CompositeItemEffect.generated.h"

enum class EDataValidationResult : uint8;
class AIdkCharacter;
class FDataValidationContext;
class FName;
class UItemEffect;
class UObject;

/** Item effect that groups together other item effects. */
UCLASS()
class UCompositeItemEffect final : public UItemEffect
{
	GENERATED_BODY()

	friend class FCompositeItemEffectBuilder;

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
	/** List of subeffects. */
	UPROPERTY(EditAnywhere, Instanced, NoClear/*, meta = (EditCondition = "!bBegunPlay", HideEditConditionToggle)*/)
	TArray<UItemEffect*> Effects;
};
