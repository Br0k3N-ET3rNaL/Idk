// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectSavedValues.h"

#include "Idk/EffectSystem/EffectId.h"

FEffectSavedValues::FEffectSavedValues(const bool bFromMultiHit)
	: bFromMultiHit(bFromMultiHit)
{
}

void FEffectSavedValues::SaveValue(const FEffectId SaveId, const double Value)
{
	SavedValues.FindOrAdd(SaveId) += Value;
}

double FEffectSavedValues::GetSavedValue(const FEffectId SaveId) const
{
	return *SavedValues.Find(SaveId);
}

bool FEffectSavedValues::HasSavedValues() const
{
	return !SavedValues.IsEmpty();
}

bool FEffectSavedValues::IsFromMultiHitEffect() const
{
	return bFromMultiHit;
}
