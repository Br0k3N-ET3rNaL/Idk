// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/HealPickup.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Misc/AssertionMacros.h>

void AHealPickup::Init(const double HealAmount)
{
	check(HealAmount > 0.0);

	HealEffect = &FSimpleEffectBuilder::Begin(this)
		.InitHeal(HealAmount)
		.Complete();
}

void AHealPickup::OnPickup(AIdkPlayerCharacter& Player)
{
	FApplyEffectParams Params = FApplyEffectParams(nullptr, Player.GetAttributeSystem());

	HealEffect->ApplyEffect(Params);
}
