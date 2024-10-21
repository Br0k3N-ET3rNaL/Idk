// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/MultiStageEffect.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/BonusEffect.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectIdSet.h"
#include "Idk/EffectSystem/EffectSavedValues.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/MultiStageBonusEffect.h"
#include "Idk/EffectSystem/MultiStageEffectContainer.h"
#include "Idk/EffectSystem/PartialEffectInfo.h"
#include "Idk/EffectSystem/SimpleEffect.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/UI/TooltipFormatParam.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Misc/EnumRange.h>
#include <Templates/Casts.h>
#include <Templates/PimplPtr.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString UMultiStageEffect::EffectClassName = TEXT("MultiEffect");

#if WITH_EDITOR
EDataValidationResult UMultiStageEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	FString InvalidReason;

	if (!Effects.IsValid(InvalidReason))
	{
		Context.AddError(FText::FromString(InvalidReason));

		Result = EDataValidationResult::Invalid;
	}

	// All previously saved IDs
	TSet<FEffectId> SaveIds;

	// All previously used IDs
	TSet<FUseIdInfo> UseIds;

	// All previously saved IDs in the current stage
	TSet<FEffectId> StageSaveIds;

	// All previously used IDs in the current stage
	TSet<FUseIdInfo> StageUseIds;

	const FText InvalidSelfEffectPrefix = FText::FromString(TEXT("A SelfEffect was invalid: "));
	const FText InvalidTargetEffectPrefix = FText::FromString(TEXT("A TargetEffect was invalid: "));
	const FText InvalidCallbackEffectPrefix = FText::FromString(TEXT("A CallbackEffect was invalid: "));

	for (auto EffectIt = Effects.CreateConstStageIterator(EEffectStage::Self); EffectIt; ++EffectIt)
	{
		const USingleStageEffect* Effect = *EffectIt;

		if (!Effect)
		{
			Context.AddError(FText::FromString(TEXT("A SelfEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (Effect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidSelfEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			const TSet<FUseIdInfo>& EffectUseIds = Effect->GetUseIds();

			TSet<FEffectId> UsedBeforeSavingIds;

			for (const FUseIdInfo& UseIdInfo : EffectUseIds)
			{
				if (!UseIdInfo.bUseLocalValue)
				{
					Context.AddError(FText::FromString(FString::Printf(
						TEXT("bUseLocalValue for ID = %u was false, but SelfEffects is the first stage so they can only use local values"),
						UseIdInfo.UseId)));

					Result = EDataValidationResult::Invalid;
				}

				if (!StageSaveIds.Contains(UseIdInfo.UseId))
				{
					UsedBeforeSavingIds.Add(UseIdInfo.UseId);
				}
			}

			if (!UsedBeforeSavingIds.IsEmpty() && !Effect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A SelfEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(Effect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);
	StageSaveIds.Reset();
	StageUseIds.Reset();

	for (auto EffectIt = Effects.CreateConstStageIterator(EEffectStage::Target); EffectIt; ++EffectIt)
	{
		const USingleStageEffect* Effect = *EffectIt;

		if (!Effect)
		{
			Context.AddError(FText::FromString(TEXT("A TargetEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (Effect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidTargetEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			const TSet<FUseIdInfo>& EffectUseIds = Effect->GetUseIds();

			TSet<FEffectId> UsedBeforeSavingIds;

			for (const FUseIdInfo& UseIdInfo : EffectUseIds)
			{
				if ((UseIdInfo.bUseLocalValue && !StageSaveIds.Contains(UseIdInfo.UseId)) ||
					(!UseIdInfo.bUseLocalValue && !SaveIds.Contains(UseIdInfo.UseId)))
				{
					UsedBeforeSavingIds.Add(UseIdInfo.UseId);
				}
			}

			if (!UsedBeforeSavingIds.IsEmpty() && !Effect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A TargetEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(Effect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);
	StageSaveIds.Reset();
	StageUseIds.Reset();

	for (auto EffectIt = Effects.CreateConstStageIterator(EEffectStage::Callback); EffectIt; ++EffectIt)
	{
		const USingleStageEffect* Effect = *EffectIt;

		if (!Effect)
		{
			Context.AddError(FText::FromString(TEXT("A CallbackEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (Effect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidCallbackEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			const TSet<FUseIdInfo>& EffectUseIds = Effect->GetUseIds();

			TSet<FEffectId> UsedBeforeSavingIds;

			for (const FUseIdInfo& UseIdInfo : EffectUseIds)
			{
				if ((UseIdInfo.bUseLocalValue && !StageSaveIds.Contains(UseIdInfo.UseId)) ||
					(!UseIdInfo.bUseLocalValue && !SaveIds.Contains(UseIdInfo.UseId)))
				{
					UsedBeforeSavingIds.Add(UseIdInfo.UseId);
				}
			}

			if (!UsedBeforeSavingIds.IsEmpty() && !Effect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A CallbackEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(Effect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);

	return Result;
}
#endif

UEffect* UMultiStageEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UMultiStageEffect* NewEffect = DuplicateBaseEffect<UMultiStageEffect>(Outer, Name);

	for (const EEffectStage Stage : TEnumRange<EEffectStage>())
	{
		for (auto EffectIt = Effects.CreateConstStageIterator(Stage); EffectIt; ++EffectIt)
		{
			NewEffect->Effects.AddToStage(*UEffect::DuplicateEffect(**EffectIt, Outer, *this, *NewEffect), Stage);
		}
	}

	NewEffect->BonusEffectIds = BonusEffectIds;
	NewEffect->bCanHaveTargetEffects = bCanHaveTargetEffects;

	return NewEffect;
}

void UMultiStageEffect::InitDescriptions(UAttributeSystemComponent& AttributeSystem)
{
	for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
	{
		(*EffectIt)->InitDescriptions(AttributeSystem);
	}
}

void UMultiStageEffect::ApplyEffectsToSelf(UAttributeSystemComponent& Self, const double ConditionalEffectValue) const
{
	SelfSavedValues = MakePimpl<FEffectSavedValues>(false);

	SelfSavedValues->SaveValue(FEffectId::ConditionalEffectValueId, ConditionalEffectValue);

	const bool bUseModifiers = false;
	const double PerHitMultiplier = 1.0;

	FApplyEffectParams ApplyEffectParams 
		= FApplyEffectParams(&Self, &Self, PerHitMultiplier, bUseModifiers, ETriggerEffects::None, nullptr, SelfSavedValues.Get());

	ApplyStageEffects(EEffectStage::Self, ApplyEffectParams);
}

void UMultiStageEffect::ApplyEffectsToSelf(UAttributeSystemComponent& Self, const FVector2D EffectOrigin) const
{
	SelfSavedValues = MakePimpl<FEffectSavedValues>(false);

	const bool bUseModifiers = true;
	const double PerHitMultiplier = 1.0;

	FApplyEffectParams ApplyEffectParams 
		= FApplyEffectParams(&Self, &Self, EffectOrigin, PerHitMultiplier, bUseModifiers, ETriggerEffects::Friendly, nullptr, SelfSavedValues.Get());

	ApplyStageEffects(EEffectStage::Self, ApplyEffectParams);
}

void UMultiStageEffect::ApplyEffectsToTargets(UAttributeSystemComponent& Self, TArray<UAttributeSystemComponent*>& Targets, const FVector2D EffectOrigin, const double PerHitMultiplier) const
{
	for (UAttributeSystemComponent* Target : Targets)
	{
		ApplyEffectsToTarget(Self, *Target, EffectOrigin, PerHitMultiplier);
	}
}

void UMultiStageEffect::ApplyEffectsToTarget(UAttributeSystemComponent& Self, UAttributeSystemComponent& Target, const FVector2D EffectOrigin, const double PerHitMultiplier) const
{
	FEffectSavedValues TargetSavedValues;

	const bool bUseModifiers = true;

	FApplyEffectParams TargetParams 
		= FApplyEffectParams(&Self, &Target, EffectOrigin, PerHitMultiplier, bUseModifiers, ETriggerEffects::Hostile, SelfSavedValues.Get(), &TargetSavedValues);

	ApplyStageEffects(EEffectStage::Target, TargetParams);

	FEffectSavedValues CallbackSavedValues;

	FApplyEffectParams CallbackParams 
		= FApplyEffectParams(&Self, &Self, EffectOrigin, PerHitMultiplier, bUseModifiers, ETriggerEffects::Friendly, &TargetSavedValues, &CallbackSavedValues);

	ApplyStageEffects(EEffectStage::Callback, CallbackParams);
}

void UMultiStageEffect::ApplyEffectsToTarget(UAttributeSystemComponent& Self, UAttributeSystemComponent& Target, const double ConditionalEffectValue) const
{
	FEffectSavedValues TargetSavedValues;

	TargetSavedValues.SaveValue(FEffectId::ConditionalEffectValueId, ConditionalEffectValue);

	const bool bUseModifiers = false;

	FApplyEffectParams TargetParams 
		= FApplyEffectParams(&Self, &Target, 1.0, bUseModifiers, ETriggerEffects::None, SelfSavedValues.Get(), &TargetSavedValues);

	ApplyStageEffects(EEffectStage::Target, TargetParams);

	FEffectSavedValues CallbackSavedValues;

	FApplyEffectParams CallbackParams 
		= FApplyEffectParams(&Self, &Self, 1.0, bUseModifiers, ETriggerEffects::None, &TargetSavedValues, &CallbackSavedValues);

	ApplyStageEffects(EEffectStage::Callback, CallbackParams);
}

void UMultiStageEffect::AddBonusEffects(const UMultiStageBonusEffect& InBonusEffect, FEffectId& InOutEffectId, const bool bNewId)
{
	if (CanHaveTargetEffects() || InBonusEffect.Effects.IsStageEmpty(EEffectStage::Target))
	{
		// Check if the ID is already set
		if (bNewId || InOutEffectId == FEffectId::NoId)
		{
			// Copy the multistage bonus effect
			UMultiStageBonusEffect* BonusEffects = UEffect::DuplicateEffect(InBonusEffect, GetOuter());

			check(BonusEffectIds.IsEmpty() || BonusEffects->Effects.Num() <= 1);

			if (BonusEffects->BonusEffectIds.Num() <= 1)
			{
				if (InOutEffectId == FEffectId::NoId)
				{
					InOutEffectId = BonusEffectIds.GetNextValidId();

					BonusEffects->SetBonusEffectId(InOutEffectId);
				}

				check(!BonusEffectIds.Contains(InOutEffectId));

				BonusEffectIds.Add(InOutEffectId);
			}
			else
			{
				BonusEffectIds.Append(BonusEffects->BonusEffectIds);
			}

			TSet<FEffectId> SaveIds = GetSaveIds();
			TSet<FEffectId> OtherSaveIds = BonusEffects->GetSaveIds();

			TSet<FEffectId> OverlappingIds = SaveIds.Intersect(OtherSaveIds);

			OverlappingIds.Remove(FEffectId::NoId);

			if (!OverlappingIds.IsEmpty())
			{
				TSet<FEffectId> ValidSaveIds = FEffectIdSet::GetValidIds(SaveIds.Union(OtherSaveIds), OverlappingIds.Num());

				// Resolve any overlapping effect IDs
				BonusEffects->SwapEffectIds(OverlappingIds, ValidSaveIds);
			}

			Effects.Append(BonusEffects->Effects, !CanHaveTargetEffects());
		}
		else
		{
			for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
			{
				UBonusEffect* BonusEffect = Cast<UBonusEffect>(*EffectIt);

				if (BonusEffect && BonusEffect->IsIdEqualTo(InOutEffectId))
				{
					BonusEffect->IncrementStacks();
				}
			}
		}
	}
}

void UMultiStageEffect::RemoveBonusEffects(const FEffectId BonusEffectId)
{
	for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
	{
		UBonusEffect* BonusEffect = Cast<UBonusEffect>(*EffectIt);

		if (BonusEffect && BonusEffect->IsIdEqualTo(BonusEffectId))
		{
			EffectIt.RemoveCurrent();
		}
	}
}

void UMultiStageEffect::RemoveAllBonusEffects()
{
	for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
	{
		if ((*EffectIt)->IsA<UBonusEffect>())
		{
			EffectIt.RemoveCurrent();
		}
	}
}

void UMultiStageEffect::AddPartialEffect(const FPartialEffectInfo& PartialEffectInfo, FEffectId& OutBonusEffectId, FEffectId& OutSaveId)
{
	check(OutBonusEffectId == FEffectId::NoId && OutSaveId == FEffectId::NoId);

	const EEffectStage Stage = PartialEffectInfo.GetStage();

	check(Stage != EEffectStage::Self);

	if ((Stage == EEffectStage::Target || Stage == EEffectStage::Callback) && CanHaveTargetEffects())
	{
		const UBonusEffect& InEffectToAdd = PartialEffectInfo.GetEffectToAdd();

		// Copy the bonus effect
		UBonusEffect* EffectToAdd = UEffect::DuplicateEffect(InEffectToAdd, GetOuter());

		TSet<FEffectId> SaveIds = GetSaveIds();
		TSet<FEffectId> OtherSaveIds = EffectToAdd->GetSaveIds();

		TSet<FEffectId> OverlappingIds = SaveIds.Intersect(OtherSaveIds);

		OverlappingIds.Remove(FEffectId::NoId);

		if (!OverlappingIds.IsEmpty())
		{
			TSet<FEffectId> ValidSaveIds = FEffectIdSet::GetValidIds(SaveIds.Union(OtherSaveIds), OverlappingIds.Num());

			// Resolve overlapping effect IDs
			EffectToAdd->SwapIds(FEffectIdSet::GetIdSwapMap(OverlappingIds, ValidSaveIds));
		}

		const EEffectStage PrevStage = (Stage == EEffectStage::Target) ? EEffectStage::Self : EEffectStage::Target;
		const EEffectType TypeToAddEffectTo = PartialEffectInfo.GetTypeToAddEffectTo();

		TArray<USimpleEffect*> EffectsToModify;

		for (auto EffectIt = Effects.CreateStageIterator(PrevStage); EffectIt; ++EffectIt)
		{
			(*EffectIt)->GetEffectsByType(TypeToAddEffectTo, EffectsToModify);
		}

		OutSaveId = FEffectIdSet::GetNextValidId(GetSaveIds());

		for (USimpleEffect* Effect : EffectsToModify)
		{
			Effect->AddBonusSaveId(OutSaveId);
		}

		TMap<FEffectId, FEffectId> SwapMap;

		SwapMap.Add(FEffectId::PartialEffectPlaceholderId, OutSaveId);

		// Swap partial effect placeholder IDs for actual ID
		EffectToAdd->SwapIds(SwapMap);

		Effects.AddToStage(*EffectToAdd, Stage);
	}
}

void UMultiStageEffect::AddPartialEffectStack(const FPartialEffectInfo& PartialEffectInfo, const FEffectId BonusEffectId)
{
	check(BonusEffectId != FEffectId::NoId);
	check(BonusEffectIds.Contains(BonusEffectId));

	const EEffectStage Stage = PartialEffectInfo.GetStage();

	for (auto EffectIt = Effects.CreateStageIterator(Stage); EffectIt; ++EffectIt)
	{
		UBonusEffect* BonusEffect = Cast<UBonusEffect>(*EffectIt);

		if (BonusEffect && BonusEffect->IsIdEqualTo(BonusEffectId))
		{
			BonusEffect->IncrementStacks();
		}
	}
}

void UMultiStageEffect::RemovePartialEffect(const FPartialEffectInfo& PartialEffectInfo, const FEffectId BonusEffectId, const FEffectId SaveId)
{
	const EEffectStage Stage = PartialEffectInfo.GetStage();

	check(BonusEffectId != FEffectId::NoId && SaveId != FEffectId::NoId);
	check(Stage != EEffectStage::Self);

	const EEffectStage PrevStage = (Stage == EEffectStage::Target) ? EEffectStage::Self : EEffectStage::Target;

	TArray<USimpleEffect*> EffectsToModify;

	for (auto EffectIt = Effects.CreateStageIterator(PrevStage); EffectIt; ++EffectIt)
	{
		(*EffectIt)->GetEffectsByType(PartialEffectInfo.GetTypeToAddEffectTo(), EffectsToModify);
	}

	for (USimpleEffect* Effect : EffectsToModify)
	{
		Effect->RemoveBonusSaveId(SaveId);
	}

	for (auto EffectsIt = Effects.CreateStageIterator(Stage); EffectsIt; ++EffectsIt)
	{
		UBonusEffect* BonusEffect = Cast<UBonusEffect>(*EffectsIt);

		if (BonusEffect && BonusEffect->IsIdEqualTo(BonusEffectId))
		{
			EffectsIt.RemoveCurrent();
		}
	}
}

void UMultiStageEffect::AllowAddingTargetEffects()
{
	bCanHaveTargetEffects = true;
}

bool UMultiStageEffect::HasEffects() const
{
	return !Effects.IsEmpty();
}

bool UMultiStageEffect::CanHaveTargetEffects() const
{
	return !Effects.IsStageEmpty(EEffectStage::Target) || bCanHaveTargetEffects;
}

void UMultiStageEffect::GetTooltipParams(FFormatNamedArguments& Args, UAttributeSystemComponent* Self, const double Duration, const double Interval) const
{
	TMap<FName, FText> StatusEffectParams;
	TMap<EEffectStage, TMap<EEffectType, double>> EffectValueParams;

	for (const EEffectStage Stage : TEnumRange<EEffectStage>())
	{
		EffectValueParams.Add(Stage);
	}

	for (TPair<FString, FFormatArgumentValue>& NameValuePair : Args)
	{
		if (FTooltipFormatParam::IsMultiStageEffectValue(NameValuePair.Key))
		{
			const EEffectStage Stage = FTooltipFormatParam::GetEffectStage(NameValuePair.Key);
			const EEffectType EffectType = FTooltipFormatParam::GetEffectType(NameValuePair.Key);

			EffectValueParams[Stage].Add(EffectType, 0.0);
		}
		else if (FTooltipFormatParam::IsStatusEffect(NameValuePair.Key))
		{
			StatusEffectParams.Add(FTooltipFormatParam::GetStatusEffectName(NameValuePair.Key));
		}
		else if (FTooltipFormatParam::IsDuration(NameValuePair.Key))
		{
			NameValuePair.Value = FText::AsNumber(Duration);
		}
		else if (FTooltipFormatParam::IsInterval(NameValuePair.Key))
		{
			NameValuePair.Value = FText::AsNumber(Interval);
		}
	}

	const double PerHitMultiplier = (Duration > 0.0) ? Interval / Duration : 1.0;

	FApplyEffectParams EffectParams = FApplyEffectParams(Self, nullptr, PerHitMultiplier, true);

	for (const EEffectStage Stage : TEnumRange<EEffectStage>())
	{
		for (auto EffectIt = Effects.CreateConstStageIterator(Stage); EffectIt; ++EffectIt)
		{
			(*EffectIt)->GetTooltipParams(EffectParams, StatusEffectParams, EffectValueParams[Stage]);
		}
	}

	for (const TPair<FName, FText>& Pair : StatusEffectParams)
	{
		Args[FTooltipFormatParam::FromStatusEffect(Pair.Key)] = Pair.Value;
	}

	for (const TPair<EEffectStage, TMap<EEffectType, double>>& OuterPair : EffectValueParams)
	{
		for (const TPair<EEffectType, double>& TypeValuePair : OuterPair.Value)
		{
			Args[FTooltipFormatParam::FromMultiStageEffectValue(OuterPair.Key, TypeValuePair.Key)] = FText::AsNumber(TypeValuePair.Value);
		}
	}
}

void UMultiStageEffect::ApplyStageEffects(const EEffectStage Stage, FApplyEffectParams& Params) const
{
	for (auto EffectIt = Effects.CreateConstStageIterator(Stage); EffectIt; ++EffectIt)
	{
		(*EffectIt)->ApplyEffect(Params);
	}
}

TSet<FEffectId> UMultiStageEffect::GetSaveIds() const
{
	TSet<FEffectId> SaveIds;

	for (auto EffectIt = Effects.CreateConstIterator(); EffectIt; ++EffectIt)
	{
		SaveIds.Append((*EffectIt)->GetSaveIds());
	}

	return SaveIds;
}
