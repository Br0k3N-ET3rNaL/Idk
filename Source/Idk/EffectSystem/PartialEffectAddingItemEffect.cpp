// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/PartialEffectAddingItemEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/BonusEffect.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString UPartialEffectAddingItemEffect::EffectClassName = TEXT("PartialEffect");

#if WITH_EDITOR
EDataValidationResult UPartialEffectAddingItemEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (Info.Location != EAddEffectLocation::BasicAttack && Info.Location != EAddEffectLocation::Abilities)
	{
		Context.AddError(FText::FromString(TEXT("UPartialEffectAddingItemEffect: Can only add partial effects to basic attacks or abilities")));

		Result = EDataValidationResult::Invalid;
	}

	if (Info.Stage == EEffectStage::None || Info.Stage == EEffectStage::Self)
	{
		Context.AddError(FText::FromString(TEXT("UPartialEffectAddingItemEffect: Can only add partial effects to Target or Callback stages")));

		Result = EDataValidationResult::Invalid;
	}

	if (Info.TypeToAddEffectTo != EEffectType::Damage && Info.TypeToAddEffectTo != EEffectType::Healing)
	{
		Context.AddError(FText::FromString(TEXT("UPartialEffectAddingItemEffect: Can only add partial effects to Damage or Healing effects")));

		Result = EDataValidationResult::Invalid;
	}

	if (!Info.EffectToAdd)
	{
		Context.AddError(FText::FromString(TEXT("UPartialEffectAddingItemEffect: EffectToAdd cannot be null")));

		Result = EDataValidationResult::Invalid;
	}
	else
	{
		const FText InvalidEffectPrefix = FText::FromString(TEXT("UPartialEffectAddingItemEffect: EffectToAdd was invalid: "));
		FDataValidationContext EffectContext;

		if (Info.EffectToAdd->IsEffectValid(EffectContext, Flags | EEffectValidationFlags::InPartialEffect) == EDataValidationResult::Invalid)
		{
			FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

			Result = EDataValidationResult::Invalid;
		}

		if (!Info.EffectToAdd->GetUseIds().Contains(FUseIdInfo(FEffectId::PartialEffectPlaceholderId, false)))
		{
			Context.AddError(FText::FromString(TEXT("UPartialEffectAddingItemEffect: At least one sub-effect needs to use FEffectId::PartialEffectPlaceholderId")));

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

UEffect* UPartialEffectAddingItemEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UPartialEffectAddingItemEffect* NewEffect = DuplicateBaseEffect<UPartialEffectAddingItemEffect>(Outer, Name);

	NewEffect->Info.Location = Info.Location;
	NewEffect->Info.Stage = Info.Stage;
	NewEffect->Info.TypeToAddEffectTo = Info.TypeToAddEffectTo;
	NewEffect->Info.EffectToAdd = UEffect::DuplicateEffect(*Info.EffectToAdd, Outer, *this, *NewEffect);
	NewEffect->Id = Id;

	return NewEffect;
}

void UPartialEffectAddingItemEffect::ApplyEffect(UAttributeSystemComponent* Target) const
{
	switch (Info.Location)
	{
		case EAddEffectLocation::BasicAttack:
			Target->AddPartialEffectToBasicAttack(Info, Id);
			break;
		case EAddEffectLocation::Abilities:
			Target->AddPartialEffectToAbilities(Info, Id);
			break;
		default:
			break;
	}

	Info.EffectToAdd->SetId(Id);
}

void UPartialEffectAddingItemEffect::RemoveEffect(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	switch (Info.Location)
	{
		case EAddEffectLocation::BasicAttack:
			Target->RemovePartialEffectFromBasicAttack(Info, Id);
			break;
		case EAddEffectLocation::Abilities:
			Target->RemovePartialEffectFromAbilities(Info, Id);
			break;
		default:
			break;
	}

	Id = FEffectId::NoId;
}
