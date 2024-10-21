// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h"

#include "Idk/EffectSystem/AttributeAlteringEffect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Misc/AssertionMacros.h>

FAttributeAlteringEffectBuilder& FAttributeAlteringEffectBuilder::Init(const EAttributeType AttributeToAlter, const double Bonus, const double MultiplierBonus)
{
	check(AttributeToAlter != EAttributeType::None);

	Effect->Attribute = FAttribute(true);
	Effect->Attribute.Init(AttributeToAlter, 0.0, Bonus, MultiplierBonus);

	return *this;
}
