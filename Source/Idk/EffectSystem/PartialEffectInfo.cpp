// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/EffectSystem/PartialEffectInfo.h"

EAddEffectLocation FPartialEffectInfo::GetLocation() const
{
	return Location;
}

EEffectStage FPartialEffectInfo::GetStage() const
{
	return Stage;
}

EEffectType FPartialEffectInfo::GetTypeToAddEffectTo() const
{
	return TypeToAddEffectTo;
}

const UBonusEffect& FPartialEffectInfo::GetEffectToAdd() const
{
	return *EffectToAdd;
}
