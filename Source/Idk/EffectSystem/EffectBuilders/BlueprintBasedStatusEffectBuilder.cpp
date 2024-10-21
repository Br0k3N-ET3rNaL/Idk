// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/BlueprintBasedStatusEffectBuilder.h"

#include "Idk/EffectSystem/StatusEffectBlueprintBase.h"
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

FBlueprintBasedStatusEffectBuilder& FBlueprintBasedStatusEffectBuilder::Init(TSubclassOf<UStatusEffectBlueprintBase> StatusEffectClass)
{
	Effect->bUseBlueprintClass = true;
	Effect->BlueprintClass = StatusEffectClass;

	if (!Effect->HasAllFlags(RF_DefaultSubObject))
	{
		Effect->CreateBlueprintBasedStatusEffect();
	}

	return *this;
}
