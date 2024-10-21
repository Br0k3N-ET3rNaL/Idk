// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/AbilityInfoBuilder.h"

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UnrealTemplate.h>
#include <UObject/NameTypes.h>

FAbilityInfoBuilder FAbilityInfoBuilder::Begin()
{
    return FAbilityInfoBuilder();
}

FAbilityInfoBuilder&& FAbilityInfoBuilder::Init(FName&& Name, FAbilityTargetingInfo&& TargetingInfo, const double Cooldown, const double Duration, const double HitRate)&&
{
    AbilityInfo.Name = MoveTemp(Name);
    AbilityInfo.TargetingInfo = MoveTemp(TargetingInfo);
    AbilityInfo.Cooldown = Cooldown;
    AbilityInfo.Duration = Duration;
    AbilityInfo.HitRate = HitRate;

    return MoveTemp(*this);
}

FAbilityInfoBuilder&& FAbilityInfoBuilder::SetEffect(TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder>&& Builder)&&
{
    check(!AbilityInfo.Name.IsNone());

    AbilityInfo.Effect = &Builder.Complete(FString::Printf(TEXT("%sEffect"), *AbilityInfo.Name.ToString()));

#if WITH_EDITOR
    AbilityInfo.Effect->CheckIsValidEffect(EEffectValidationFlags::InitialValidation);
#endif

    return MoveTemp(*this);
}

FAbilityInfoBuilder&& FAbilityInfoBuilder::SetDisplayInfo(FGenericDisplayInfo&& DisplayInfo)&&
{
    AbilityInfo.DisplayInfo = MoveTemp(DisplayInfo);

    return MoveTemp(*this);
}

FAbilityInfo&& FAbilityInfoBuilder::Complete()&&
{
    return MoveTemp(AbilityInfo);
}
