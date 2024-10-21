// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/PartialEffectAddingEffectBuilder.h"

#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/PartialEffectAddingItemEffect.h"
#include "Idk/EffectSystem/PartialEffectInfo.h"

FPartialEffectAddingEffectBuilder& FPartialEffectAddingEffectBuilder::Init(const EAddEffectLocation Location, const EEffectStage Stage, const EEffectType TypeToAddEffectTo)
{
	Effect->Info.Location = Location;
	Effect->Info.Stage = Stage;
	Effect->Info.TypeToAddEffectTo = TypeToAddEffectTo;

	return *this;
}
