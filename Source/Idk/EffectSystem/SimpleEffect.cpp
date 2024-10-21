// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/SimpleEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectSavedValues.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <CoreGlobals.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Logging/LogMacros.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Function.h>
#include <UObject/Class.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

FEffectVariableValueInfo::FEffectVariableValueInfo(const double Weight, const bool bUseValueAsMultiplier, const bool bUseLocalValue)
	: Weight(Weight), bUseValueAsMultiplierBonus(bUseValueAsMultiplier), bUseLocalValue(bUseLocalValue)
{
}

double FEffectVariableValueInfo::GetWeight() const
{
	return Weight;
}

bool FEffectVariableValueInfo::UsesValueAsMultiplierBonus() const
{
	return bUseValueAsMultiplierBonus;
}

bool FEffectVariableValueInfo::UsesLocalValue() const
{
	return bUseLocalValue;
}

const FName USimpleEffect::AnyStatusEffect = TEXT("Any");

const FString USimpleEffect::EffectClassName = TEXT("SimpleEffect");

#if WITH_EDITOR
EDataValidationResult USimpleEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (FEffectId::IsReservedId(SaveId))
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("USimpleEffect: Invalid save ID, %i is a reserved id"), SaveId)));

		Result = EDataValidationResult::Invalid;
	}

	switch (Type)
	{
		case EEffectType::None:
			Context.AddError(FText::FromString(TEXT("USimpleEffect: Type is None")));

			Result = EDataValidationResult::Invalid;
			break;
		case EEffectType::Damage:
		case EEffectType::Healing:
			if (BaseValue < 0.0)
			{
				Context.AddError(FText::FromString(TEXT("USimpleEffect: BaseValue should be greater than 0 for damage and healing")));
				
				Result = EDataValidationResult::Invalid;
			}
			[[fallthrough]];
		case EEffectType::Push:
			if ((Flags & EEffectValidationFlags::InPartialEffect) == EEffectValidationFlags::None
				&& BaseValue == 0.0 && ValuesToUse.IsEmpty())
			{
				Context.AddError(FText::FromString(TEXT("USimpleEffect: BaseValue is 0.0 and effect doesn't use any variable values, so the effect will do nothing")));
				
				Result = EDataValidationResult::Invalid;
			}
			break;
		case EEffectType::GetStatusEffectStacks:
			if (NameForEffect.IsNone())
			{
				Context.AddError(FText::FromString(TEXT("USimpleEffect: GetStatusEffectStacks needs to have NameForEffect set")));

				Result = EDataValidationResult::Invalid;
			}
			[[fallthrough]];
		case EEffectType::GetMissingHealth:
		case EEffectType::GetHealthPercent:
			if (SaveId == FEffectId::NoId)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("USimpleEffect: Effect of type %s need to save their values"), *UEnum::GetValueAsString(Type))));
				
				Result = EDataValidationResult::Invalid;
			}
			break;
		case EEffectType::RemoveStatusEffect:
			if (NameForEffect.IsNone() && !bRemoveAllStatusEffects)
			{
				Context.AddError(FText::FromString(TEXT("USimpleEffect: RemoveStatusEffect needs to have NameForEffect set, or bRemoveAllStatusEffects")));

				Result = EDataValidationResult::Invalid;
			}
			break;
		case EEffectType::DisableItem:
			if (NameForEffect.IsNone())
			{
				Context.AddError(FText::FromString(TEXT("USimpleEffect: DisableItem needs to have NameForEffect set")));

				Result = EDataValidationResult::Invalid;
			}
		default:
			break;
	}

	for (const TPair<FEffectId, FEffectVariableValueInfo>& ValueToUse : ValuesToUse)
	{
		if (ValueToUse.Value.GetWeight() <= 0.0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("USimpleEffect: Uses value with ID %u, but weight is <= 0.0, so it won't have any effect"), ValueToUse.Key)));

			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

UEffect* USimpleEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	USimpleEffect* NewEffect = DuplicateBaseEffect<USimpleEffect>(Outer, Name);

	NewEffect->Type = Type;
	NewEffect->BaseValue = BaseValue;
	NewEffect->NameForEffect = NameForEffect;
	NewEffect->SaveId = SaveId;
	NewEffect->BonusSaveIds = BonusSaveIds;
	NewEffect->ValuesToUse = ValuesToUse;
	NewEffect->bRemoveAllStatusEffects = bRemoveAllStatusEffects;
	NewEffect->bUseModifiers = bUseModifiers;

	return NewEffect;
}

void USimpleEffect::InitDescriptions(UAttributeSystemComponent& AttributeSystem)
{
	// Simple effects don't have descriptions
}

void USimpleEffect::ApplyEffect(FApplyEffectParams& Params) const
{
	const double SavedValue = InternalApplyEffect(Params);

	if (SaveId != FEffectId::NoId)
	{
		Params.CurrentSavedValues->SaveValue(SaveId, SavedValue);
	}

	for (const FEffectId BonusSaveId : BonusSaveIds)
	{
		Params.CurrentSavedValues->SaveValue(BonusSaveId, SavedValue);
	}
}

TSet<FEffectId> USimpleEffect::GetSaveIds() const
{
	TSet<FEffectId> SaveIds = TSet<FEffectId>(BonusSaveIds);
	SaveIds.Add(SaveId);

	return SaveIds;
}

TSet<FUseIdInfo> USimpleEffect::GetUseIds() const
{
	TSet<FUseIdInfo> UseIdInfo;

	for (const TPair<FEffectId, FEffectVariableValueInfo>& ValueToUse : ValuesToUse)
	{
		UseIdInfo.Add(FUseIdInfo(ValueToUse.Key, ValueToUse.Value.UsesLocalValue()));
	}

	return UseIdInfo;
}

bool USimpleEffect::SavesBeforeUsingIds(const TSet<FEffectId>& Ids) const
{
	// Simple effects are only a single effect so the order of saving and using values doesn't matter

	return true;
}

void USimpleEffect::SwapIds(const TMap<FEffectId, FEffectId>& IdMap)
{
	if (const FEffectId* NewId = IdMap.Find(SaveId))
	{
		SaveId = *NewId;
	}

	// Swap bonus save IDs
	for (FEffectId& BonusSaveId : BonusSaveIds)
	{
		if (const FEffectId* NewId = IdMap.Find(BonusSaveId))
		{
			BonusSaveId = *NewId;
		}
	}

	TMap<FEffectId, FEffectVariableValueInfo> NewValuesToUse;

	// Swap use IDs
	for (const TPair<FEffectId, FEffectVariableValueInfo>& OldValueToUse : ValuesToUse)
	{
		if (const FEffectId* NewId = IdMap.Find(OldValueToUse.Key))
		{
			NewValuesToUse.Add(*NewId, OldValueToUse.Value);
		}
		else
		{
			NewValuesToUse.Add(OldValueToUse);
		}
	}

	ValuesToUse = NewValuesToUse;
}

void USimpleEffect::GetEffectsByType(const EEffectType EffectType, TArray<USimpleEffect*>& OutEffects)
{
	if (Type == EffectType)
	{
		OutEffects.Add(this);
	}
}

void USimpleEffect::GetTooltipParams(const FApplyEffectParams& EffectParams, TMap<FName, FText>& StatusEffectParams, TMap<EEffectType, double>& EffectValueParams) const
{
	if (double* Value = EffectValueParams.Find(Type))
	{
		(*Value) += GetEffectValue(EffectParams);
	}
}

void USimpleEffect::RemoveEffectsByUseId(const FEffectId UseId, TFunctionRef<void()> RemoveSelf)
{
	if (ValuesToUse.Contains(UseId))
	{
		RemoveSelf();
	}
}

void USimpleEffect::AddVariableToUse(const uint8 UseId, const FEffectVariableValueInfo Info)
{
	ValuesToUse.Add(UseId, Info);
}

void USimpleEffect::AddBonusSaveId(const uint8 BonusSaveId)
{
	BonusSaveIds.Add(BonusSaveId);
}

void USimpleEffect::RemoveBonusSaveId(const uint8 BonusSaveId)
{
	BonusSaveIds.Remove(BonusSaveId);
}

EAttributeType USimpleEffect::GetModifierType() const
{
	switch (Type)
	{
		case EEffectType::Damage:
			return EAttributeType::Damage;
		case EEffectType::Healing:
			return EAttributeType::Healing;
		default:
			return EAttributeType::None;
	}
}

double USimpleEffect::InternalApplyEffect(FApplyEffectParams& Params) const
{
	double SavedValue = 0.0;

	switch (Type)
	{
		case EEffectType::Damage:
		{
			check(!Params.bUseModifiers || Params.Applier);

			SavedValue = Params.Target->ApplyDamage(Params.Applier, GetEffectValue(Params), Params.EffectsToTrigger == ETriggerEffects::Hostile);

			break;
		}
		case EEffectType::Healing:
		{
			check(!Params.bUseModifiers || Params.Applier);

			SavedValue = Params.Target->ApplyHealing(GetEffectValue(Params), Params.EffectsToTrigger == ETriggerEffects::Friendly);
			
			break;
		}
		case EEffectType::GetMissingHealth:
			SavedValue = Params.Target->GetMissingHealth();
			break;
		case EEffectType::GetHealthPercent:
			SavedValue = Params.Target->GetHealthPercent();
			break;
		case EEffectType::GetStatusEffectStacks:
			SavedValue = Params.Target->GetStatusEffectStacks(NameForEffect);
			break;
		case EEffectType::RemoveStatusEffect:
			SavedValue = Params.Target->RemoveStatusEffects((bRemoveAllStatusEffects) ? AnyStatusEffect : NameForEffect);
			break;
		case EEffectType::Push:
			check(Params.bEffectOriginSet);
			Params.Target->Push(Params.EffectOrigin, GetEffectValue(Params) * PushMultiplier);
			break;
		case EEffectType::DisableItem:
			Params.Target->DisableItemByName(NameForEffect);
		default:
			break;
	}

	return SavedValue;
}

double USimpleEffect::GetEffectValue(const FApplyEffectParams& Params) const
{
	// Bonus to the effect's value
	double Bonus = 0.0;

	// Multiplied by the sum of the effect's value and bonus to get the final value
	double ValueMultiplier = 1.0;

	for (const TPair<uint8, FEffectVariableValueInfo>& ValueToUse : ValuesToUse)
	{
		check(ValueToUse.Value.UsesLocalValue() && Params.CurrentSavedValues ||
			!ValueToUse.Value.UsesLocalValue() && Params.PreviousSavedValues);

		const FEffectSavedValues& SavedValues = 
			(ValueToUse.Value.UsesLocalValue()) ? *Params.CurrentSavedValues : *Params.PreviousSavedValues;

		const FEffectVariableValueInfo& VariableValueInfo = ValueToUse.Value;
		const double VariableValue = SavedValues.GetSavedValue(ValueToUse.Key);

		if (VariableValueInfo.UsesValueAsMultiplierBonus())
		{
			ValueMultiplier += VariableValue * VariableValueInfo.GetWeight();
		}
		else
		{
			Bonus += VariableValue * VariableValueInfo.GetWeight() 
					* ((SavedValues.IsFromMultiHitEffect()) ? 1.0 : Params.PerHitMultiplier);
		}
	}

	const EAttributeType TypeModifier = GetModifierType();

	// Check if modifiers should be applied
	if (Params.bUseModifiers && bUseModifiers && FAttribute::IsAModifier(TypeModifier))
	{
		Bonus += Params.Applier->GetModifierBonus(TypeModifier) * Params.PerHitMultiplier;
		ValueMultiplier += Params.Applier->GetModifierMultiplierBonus(TypeModifier);
	}

	return ((BaseValue * Params.PerHitMultiplier) + Bonus) * ValueMultiplier;
}
