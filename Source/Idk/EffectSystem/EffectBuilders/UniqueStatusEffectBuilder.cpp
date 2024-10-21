// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectBuilders/UniqueStatusEffectBuilder.h"

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include "Idk/EffectSystem/StatusEffectInternal.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Templates/UnrealTemplate.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

FUniqueStatusEffectBuilder& FUniqueStatusEffectBuilder::SetDisplayInfo(const FName& Name, FGenericDisplayInfo&& DisplayInfo)
{
	Effect->StatusEffect->Name = Name;
	Effect->StatusEffect->DisplayInfo = MoveTemp(DisplayInfo);

	return *this;
}

FUniqueStatusEffectBuilder& FUniqueStatusEffectBuilder::SetInfo(const double Duration, const uint8 MaxStacks, const bool bRefreshable)
{
	Effect->StatusEffect->Duration = Duration;
	Effect->StatusEffect->MaxStacks = MaxStacks;
	Effect->StatusEffect->bRefreshable = bRefreshable;

	return *this;;
}

FUniqueStatusEffectBuilder& FUniqueStatusEffectBuilder::SetInfoPermanent(const uint8 MaxStacks)
{
	Effect->StatusEffect->Duration = UStatusEffectInternal::PermanentDuration;
	Effect->StatusEffect->MaxStacks = MaxStacks;
	Effect->StatusEffect->bRefreshable = false;

	return *this;
}

void FUniqueStatusEffectBuilder::PostBegin()&
{
	const FString EffectName = FString::Printf(TEXT("%s_Internal"), *Effect->GetFName().ToString());

	Effect->StatusEffect = (Effect->HasAllFlags(RF_DefaultSubObject)) 
		? UEffect::CreateEffectDefault<UStatusEffectInternal>(Effect->GetOuter(), *EffectName)
		: UEffect::CreateEffect<UStatusEffectInternal>(Effect->GetOuter(), *EffectName);
}

void FUniqueStatusEffectBuilder::PreComplete()&
{
	if (RepeatedEffectsBuilder)
	{
		Effect->StatusEffect->RepeatedEffects = &RepeatedEffectsBuilder->Complete();

		RepeatedEffectsBuilder.Reset();
	}

	if (PersistentEffectsBuilder)
	{
		Effect->StatusEffect->PersistentEffects = &PersistentEffectsBuilder->Complete();

		PersistentEffectsBuilder.Reset();
	}
}
