// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/Effect.h"

#if WITH_EDITOR
#include <Engine/World.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/Object.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>
#endif

#if WITH_EDITOR
EDataValidationResult UEffect::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (IsEffectValid(Context) == EDataValidationResult::Invalid)
	{
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

bool UEffect::CanEditChange(const FProperty* InProperty) const
{
	return !GWorld || !GWorld->HasBegunPlay();
}

EDataValidationResult UEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	return EDataValidationResult::NotValidated;
}

void UEffect::CheckIsValidEffect(const EEffectValidationFlags Flags) const
{
	FDataValidationContext Context;

	check(IsEffectValid(Context, Flags) != EDataValidationResult::Invalid);
}
#endif
