// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/StatusEffectInternal.h"
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>

#include "StatusEffectBlueprintBase.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;

/** Base class for status effects used in the status effect table. */
UCLASS(Abstract, Blueprintable, NotEditInlineNew)
class UStatusEffectBlueprintBase final : public UStatusEffectInternal
{
	GENERATED_BODY()
	
public:
	//~ Begin UStatusEffectInternal Interface.
	UE_NODISCARD virtual FText GetTooltipText() const override;
	//~ End UStatusEffectInterfal Interface
	
private:
	/** 
	 * Text used to embed the status effect's name and description within a tooltip. 
	 * 
	 * @note Wraps name with a rich text decorator that gets the description from the status effect table. 
	 */
	static const FText TooltipFormatText;
};
