// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/AttributeAlteringEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>

const FString UAttributeAlteringEffect::EffectClassName = TEXT("AttrEffect");

#if WITH_EDITOR
void UAttributeAlteringEffect::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = *PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetNameCPP();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UAttributeAlteringEffect, Attribute))
	{
		Attribute.PostEditChangeProperty(PropertyChangedEvent.Property->GetFName(), true);
	}
}

EDataValidationResult UAttributeAlteringEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (Attribute.GetAttributeType() == EAttributeType::None)
	{
		Context.AddError(FText::FromString(TEXT("UAttributeAlteringEffect: AttributeToAlter was None")));

		Result = EDataValidationResult::Invalid;

		return Result;
	}

	if (Attribute.GetBonus() == 0.0 && Attribute.GetMultiplierBonus() == 0.0)
	{
		Context.AddError(FText::FromString(TEXT("UAttributeAlteringEffect: Bonus and MultiplierBonus were both 0.0, so the effect would do nothing")));

		Result = EDataValidationResult::Invalid;
	}

	if (FAttribute::CanAttributeHaveBonus(Attribute.GetAttributeType()) != Attribute.CanHaveBonus())
	{
		Context.AddError(FText::FromString(TEXT("UAttributeAlteringEffect: bCanUseBonus doesn't match global value")));

		Result = EDataValidationResult::Invalid;
	}
	else if (FAttribute::CanAttributeHaveMultiplierBonus(Attribute.GetAttributeType()) != Attribute.CanHaveMultiplierBonus())
	{
		Context.AddError(FText::FromString(TEXT("UAttributeAlteringEffect: bCanUseMultiplierBonus doesn't match global value")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

UEffect* UAttributeAlteringEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UAttributeAlteringEffect* NewEffect = DuplicateBaseEffect<UAttributeAlteringEffect>(Outer, Name);

	NewEffect->Attribute = Attribute;

	return NewEffect;
}

void UAttributeAlteringEffect::ApplyEffect(UAttributeSystemComponent* Target) const
{
	Target->AlterAttribute(Attribute);
}

void UAttributeAlteringEffect::RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	Target->AlterAttribute(Attribute * (-1.0 * Stacks));
}
