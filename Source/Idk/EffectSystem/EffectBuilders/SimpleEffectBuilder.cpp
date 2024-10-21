// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SimpleEffect.h"
#include <Templates/UnrealTemplate.h>
#include <UObject/NameTypes.h>

FSimpleEffectBuilder& FSimpleEffectBuilder::InitDamage(const double Damage, const bool bUseModifiers)
{
	Effect->Type = EEffectType::Damage;
	Effect->BaseValue = Damage;
	Effect->bUseModifiers = bUseModifiers;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitHeal(const double HealAmount, const bool bUseModifiers)
{
	Effect->Type = EEffectType::Healing;
	Effect->BaseValue = HealAmount;
	Effect->bUseModifiers = bUseModifiers;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::SetSaveId(const FEffectId SaveId)
{
	Effect->SaveId = SaveId;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::AddValueToUse(const FEffectId UseId, FEffectVariableValueInfo&& VariableValueInfo)
{
	Effect->ValuesToUse.Add(UseId, MoveTemp(VariableValueInfo));

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitGetMissingHealth(const FEffectId SaveId)
{
	Effect->Type = EEffectType::GetMissingHealth;
	Effect->SaveId = SaveId;
	Effect->bUseModifiers = false;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitGetHealthPercent(const FEffectId SaveId)
{
	Effect->Type = EEffectType::GetHealthPercent;
	Effect->SaveId = SaveId;
	Effect->bUseModifiers = false;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitGetStatusEffectStacks(const FEffectId SaveId, const FName StatusEffectToCheckFor)
{
	Effect->Type = EEffectType::GetStatusEffectStacks;
	Effect->SaveId = SaveId;
	Effect->NameForEffect = StatusEffectToCheckFor;
	Effect->bUseModifiers = false;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitRemoveStatusEffects(const FName StatusEffectToCheckFor)
{
	Effect->Type = EEffectType::RemoveStatusEffect;
	Effect->bUseModifiers = false;
	
	if (StatusEffectToCheckFor.IsNone())
	{
		Effect->bRemoveAllStatusEffects = true;
	}
	else
	{
		Effect->NameForEffect = StatusEffectToCheckFor;
	}

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitDisableItem(const FName& ItemName)
{
	Effect->Type = EEffectType::DisableItem;
	Effect->NameForEffect = ItemName;
	Effect->bUseModifiers = false;

	return *this;
}

FSimpleEffectBuilder& FSimpleEffectBuilder::InitPush(const double Amount)
{
	Effect->Type = EEffectType::Push;
	Effect->BaseValue = Amount;
	Effect->bUseModifiers = false;

	return *this;
}
