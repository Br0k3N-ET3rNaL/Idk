// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"

#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include <UObject/Object.h>

TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder> FMultiStageEffectBuilder::CreateBasicDamageEffect(UObject* Outer, const double Damage)
{
	return FMultiStageEffectBuilder::Begin(Outer)
		.AddTargetEffect(FSimpleEffectBuilder::Begin(Outer)
			.InitDamage(Damage)
			.DeferCompletion())
		.DeferCompletion();
}

TEffectBuilderDeferred<UMultiStageEffect, FMultiStageEffectBuilder> FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(UObject* Outer, const double Damage)
{
	return FMultiStageEffectBuilder::BeginDefault(Outer)
		.AddTargetEffect(FSimpleEffectBuilder::BeginDefault(Outer)
			.InitDamage(Damage)
			.DeferCompletion())
		.DeferCompletion();
}
