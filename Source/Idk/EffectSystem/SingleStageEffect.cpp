// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/EffectSystem/SingleStageEffect.h"

#include "Idk/EffectSystem/EffectId.h"
#include <Math/MathFwd.h>

FApplyEffectParams::FApplyEffectParams(UAttributeSystemComponent* Applier, UAttributeSystemComponent* Target, const FVector2D EffectOrigin, const double PerHitMultiplier, const bool bUseModifiers, const ETriggerEffects EffectsToTrigger, const FEffectSavedValues* PreviousSavedValues, FEffectSavedValues* CurrentSavedValues)
	: Applier(Applier),
	Target(Target),
	EffectOrigin(EffectOrigin),
	PerHitMultiplier(PerHitMultiplier),
	bUseModifiers(bUseModifiers),
	EffectsToTrigger(EffectsToTrigger),
	PreviousSavedValues(PreviousSavedValues),
	CurrentSavedValues(CurrentSavedValues)
{
	bEffectOriginSet = true;
}

FApplyEffectParams::FApplyEffectParams(UAttributeSystemComponent* Applier, UAttributeSystemComponent* Target, const double PerHitMultiplier, const bool bUseModifiers, const ETriggerEffects EffectsToTrigger, const FEffectSavedValues* PreviousSavedValues, FEffectSavedValues* CurrentSavedValues)
	: Applier(Applier),
	Target(Target),
	EffectOrigin(FVector2D::ZeroVector),
	PerHitMultiplier(PerHitMultiplier),
	bUseModifiers(bUseModifiers),
	EffectsToTrigger(EffectsToTrigger),
	PreviousSavedValues(PreviousSavedValues),
	CurrentSavedValues(CurrentSavedValues)
{
}

FUseIdInfo::FUseIdInfo(const FEffectId UseId, const bool bUseLocalValue)
	: UseId(UseId), bUseLocalValue(bUseLocalValue)
{}
