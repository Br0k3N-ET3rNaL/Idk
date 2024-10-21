// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/CompositeItemEffect.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString UCompositeItemEffect::EffectClassName = TEXT("CompEffect");

#if WITH_EDITOR
EDataValidationResult UCompositeItemEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	const FText InvalidEffectPrefix = FText::FromString(TEXT("Sub-effect was invalid: "));

	for (const UItemEffect* Effect : Effects)
	{
		FDataValidationContext EffectContext;

		if (Effect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
		{
			FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

UEffect* UCompositeItemEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UCompositeItemEffect* NewEffect = DuplicateBaseEffect<UCompositeItemEffect>(Outer, Name);

	for (const UItemEffect* Effect : Effects)
	{
		NewEffect->Effects.Add(UEffect::DuplicateEffect(*Effect, Outer, *this, *NewEffect));
	}

	return NewEffect;
}

void UCompositeItemEffect::ApplyEffect(UAttributeSystemComponent* Target) const
{
	for (const UItemEffect* Effect : Effects)
	{
		Effect->ApplyEffect(Target);
	}
}

void UCompositeItemEffect::RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	for (const UItemEffect* Effect : Effects)
	{
		Effect->RemoveEffect(Target, Stacks);
	}
}
