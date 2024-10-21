// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/StatusEffect.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/EffectSystem/StatusEffectBlueprintBase.h"
#include "Idk/EffectSystem/StatusEffectInternal.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <Templates/Function.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>

const FString UStatusEffect::EffectClassName = TEXT("StatusEffect");

#if WITH_EDITOR
void UStatusEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName& PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UStatusEffect, bUseBlueprintClass)))
	{
		StatusEffect = nullptr;

		if (bUseBlueprintClass && !BlueprintClass.IsNull())
		{
			CreateBlueprintBasedStatusEffect();
		}
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UStatusEffect, BlueprintClass)) && !BlueprintClass.IsNull())
	{
		CreateBlueprintBasedStatusEffect();
	}
}
#endif

void UStatusEffect::PostLoad()
{
	Super::PostLoad();

	if (bUseBlueprintClass)
	{
		CreateBlueprintBasedStatusEffect();
	}
}

#if WITH_EDITOR
EDataValidationResult UStatusEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if ((Flags & EEffectValidationFlags::InStatusEffect) != EEffectValidationFlags::None)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("UStatusEffect: Status effects can't be nested (%s)"), *StatusEffect->GetDisplayInfo().GetDisplayName().ToString())));

		Result = EDataValidationResult::Invalid;
	}

	const FText InvalidEffectPrefix = FText::FromString(TEXT("UStatusEffect: Status effect is invalid: "));

	if (bUseBlueprintClass)
	{
		if ((Flags & EEffectValidationFlags::InitialValidation) == EEffectValidationFlags::None)
		{
			if (StatusEffect)
			{
				FDataValidationContext EffectContext;

				if (StatusEffect->IsEffectValid(EffectContext) == EDataValidationResult::Invalid)
				{
					FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

					Result = EDataValidationResult::Invalid;
				}
			}
			else if (BlueprintClass.IsNull())
			{
				Context.AddError(FText::FromString(TEXT("UStatusEffect: Blueprint class is not set.")));

				Result = EDataValidationResult::Invalid;
			}
		}
	}
	else
	{
		if (!StatusEffect)
		{
			Context.AddError(FText::FromString(TEXT("UStatusEffect: Status effect is not set.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (StatusEffect->IsEffectValid(EffectContext) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif

UEffect* UStatusEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	check(StatusEffect || bUseBlueprintClass);

	UStatusEffect* NewEffect = DuplicateBaseEffect<UStatusEffect>(Outer, Name);

	NewEffect->bUseBlueprintClass = bUseBlueprintClass;
	NewEffect->BlueprintClass = BlueprintClass;

	if (bUseBlueprintClass)
	{
		NewEffect->CreateBlueprintBasedStatusEffect();
	}
	else if (StatusEffect)
	{
		NewEffect->StatusEffect = UEffect::DuplicateEffect(*StatusEffect, Outer, *this, *NewEffect);
	}

	return NewEffect;
}

void UStatusEffect::InitDescriptions(UAttributeSystemComponent& AttributeSystem)
{
	StatusEffect->InitDescriptions(AttributeSystem);
}

void UStatusEffect::ApplyEffect(FApplyEffectParams& Params) const
{
	check(StatusEffect);

	StatusEffect->ApplyEffect(Params);
}

TSet<FEffectId> UStatusEffect::GetSaveIds() const
{
	// Status effects are self contained

	return TSet<FEffectId>();
}

TSet<FUseIdInfo> UStatusEffect::GetUseIds() const
{
	// Status effects are self contained

	return TSet<FUseIdInfo>();
}

bool UStatusEffect::SavesBeforeUsingIds(const TSet<FEffectId>& Ids) const
{
	// Status effects are self contained

	return false;
}

void UStatusEffect::SwapIds(const TMap<FEffectId, FEffectId>& IdMap)
{
	// Status effects are self contained
}

void UStatusEffect::GetEffectsByType(const EEffectType EffectType, TArray<USimpleEffect*>& OutEffects)
{
	// Status effects are self contained
}

void UStatusEffect::GetTooltipParams(const FApplyEffectParams& EffectParams, TMap<FName, FText>& StatusEffectParams, TMap<EEffectType, double>& EffectValueParams) const
{
	if (FText* Description = StatusEffectParams.Find(StatusEffect->GetName()))
	{
		(*Description) = StatusEffect->GetTooltipText();
	}
}

void UStatusEffect::RemoveEffectsByUseId(const FEffectId UseId, TFunctionRef<void()> RemoveSelf)
{
	// Status effects are self contained
}

void UStatusEffect::ActivateEffect(UAttributeSystemComponent* Target, const uint8 Stacks, const double Interval) const
{
	StatusEffect->ActivateEffect(Target, Stacks, Interval);
}

void UStatusEffect::ApplyPersistentEffects(UAttributeSystemComponent* Target) const
{
	StatusEffect->ApplyPersistentEffects(Target);
}

void UStatusEffect::RemoveAttributeAlteringEffects(UAttributeSystemComponent* Target, const uint8 Stacks) const
{
	StatusEffect->RemoveAttributeAlteringEffects(Target, Stacks);
}

const FName& UStatusEffect::GetName() const
{
	return StatusEffect->GetName();
}

const FGenericDisplayInfo& UStatusEffect::GetDisplayInfo() const
{
	return StatusEffect->GetDisplayInfo();
}

void UStatusEffect::CreateBlueprintBasedStatusEffect()
{
	check(!BlueprintClass.IsNull());

	StatusEffect = NewObject<UStatusEffectBlueprintBase>(this, BlueprintClass.LoadSynchronous(), BlueprintClass.LoadSynchronous()->GetFName(), RF_Transient);
}
