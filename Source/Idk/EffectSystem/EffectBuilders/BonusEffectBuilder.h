// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/BonusEffect.h"
#include "Idk/EffectSystem/EffectBuilders/SingleStageEffectGroupBuilder.h"

/** Builder for bonus effects. */
class FBonusEffectBuilder final : public TSingleStageEffectGroupBuilder<UBonusEffect, FBonusEffectBuilder>
{
	friend class TEffectBuilder;

	using TSingleStageEffectGroupBuilder<UBonusEffect, FBonusEffectBuilder>::TSingleStageEffectGroupBuilder;

};
