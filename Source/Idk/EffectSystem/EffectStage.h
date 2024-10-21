#pragma once 

#include <HAL/Platform.h>
#include <Misc/EnumRange.h>
#include <UObject/ObjectMacros.h>

#include "EffectStage.generated.h"

/** Represents a stage in a multistage effect. */
UENUM()
enum class EEffectStage : uint8
{
	None UMETA(Hidden),
	Self,
	Target,

	/** Applied to self after target effects have been applied. */
	Callback,
};
ENUM_RANGE_BY_VALUES(EEffectStage, EEffectStage::Self, EEffectStage::Target, EEffectStage::Callback);
