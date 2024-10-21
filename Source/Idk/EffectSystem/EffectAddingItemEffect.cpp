// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectAddingItemEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/MultiStageBonusEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString UEffectAddingItemEffect::EffectClassName = TEXT("AddEffect");

#if WITH_EDITOR
EDataValidationResult UEffectAddingItemEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	const FText InvalidEffectPrefix = FText::FromString(TEXT("UEffectAddingItemEffect: Effect was invalid: "));

	FDataValidationContext EffectContext;

	if (LocationToAddEffect == EAddEffectLocation::None)
	{
		Context.AddError(FText::FromString(TEXT("UEffectAddingItemEffect: LocationToAddEffect can't be None")));

		Result = EDataValidationResult::Invalid;
	}

	if (!Effect)
	{
		Context.AddError(FText::FromString(TEXT("UEffectAddingItemEffect: Effect not set")));

		Result = EDataValidationResult::Invalid;
	}
	else
	{
		if (Effect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
		{
			FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

			Result = EDataValidationResult::Invalid;
		}

		if (Effect->GetNumBonusEffects() > 1)
		{
			Context.AddError(FText::FromString(TEXT("UEffectAddingItemEffect: EffectAddingItemEffect can only use BonusEffects with only 1 BonusEffectId")));

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

UEffect* UEffectAddingItemEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UEffectAddingItemEffect* NewEffect = DuplicateBaseEffect<UEffectAddingItemEffect>(Outer, Name);

	NewEffect->LocationToAddEffect = LocationToAddEffect;
	NewEffect->Effect = UEffect::DuplicateEffect(*Effect, Outer, *this, *NewEffect);
	NewEffect->BonusEffectId = BonusEffectId;

	return NewEffect;
}

void UEffectAddingItemEffect::ApplyEffect(UAttributeSystemComponent* Target) const
{
	Target->AddEffectToLocation(LocationToAddEffect, *Effect, BonusEffectId);
}

void UEffectAddingItemEffect::RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	Target->RemoveEffectFromLocation(LocationToAddEffect, BonusEffectId);

	BonusEffectId = FEffectId::NoId;
}
