// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/MultiStageBonusEffect.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/BonusEffect.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectIdSet.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/MultiStageBonusEffectContainer.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Misc/EnumRange.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString UMultiStageBonusEffect::EffectClassName = TEXT("MultBonusEffect");

#if WITH_EDITOR
EDataValidationResult UMultiStageBonusEffect::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
	const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	FString InvalidReason;

	if (!Effects.IsValid(InvalidReason))
	{
		Context.AddError(FText::FromString(InvalidReason));

		Result = EDataValidationResult::Invalid;

		return Result;
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
		const UBonusEffect* BonusEffect = *EffectIt;

		if (!BonusEffect)
		{
			Context.AddError(FText::FromString(TEXT("A SelfEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (BonusEffect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidSelfEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			TSet<FUseIdInfo> EffectUseIds = BonusEffect->GetUseIds();

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

			if (!UsedBeforeSavingIds.IsEmpty() && !BonusEffect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A SelfEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(BonusEffect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);
	StageSaveIds.Reset();
	StageUseIds.Reset();

	for (auto EffectIt = Effects.CreateConstStageIterator(EEffectStage::Target); EffectIt; ++EffectIt)
	{
		const UBonusEffect* BonusEffect = *EffectIt;

		if (!BonusEffect)
		{
			Context.AddError(FText::FromString(TEXT("A TargetEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (BonusEffect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidTargetEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			TSet<FUseIdInfo> EffectUseIds = BonusEffect->GetUseIds();

			TSet<FEffectId> UsedBeforeSavingIds;

			for (const FUseIdInfo& UseIdInfo : EffectUseIds)
			{
				if ((UseIdInfo.bUseLocalValue && !StageSaveIds.Contains(UseIdInfo.UseId)) ||
					(!UseIdInfo.bUseLocalValue && !SaveIds.Contains(UseIdInfo.UseId)))
				{
					UsedBeforeSavingIds.Add(UseIdInfo.UseId);
				}
			}

			if (!UsedBeforeSavingIds.IsEmpty() && !BonusEffect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A TargetEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(BonusEffect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);
	StageSaveIds.Reset();
	StageUseIds.Reset();

	for (auto EffectIt = Effects.CreateConstStageIterator(EEffectStage::Callback); EffectIt; ++EffectIt)
	{
		const UBonusEffect* BonusEffect = *EffectIt;

		if (!BonusEffect)
		{
			Context.AddError(FText::FromString(TEXT("A CallbackEffect was null.")));

			Result = EDataValidationResult::Invalid;
		}
		else
		{
			FDataValidationContext EffectContext;

			if (BonusEffect->IsEffectValid(EffectContext, Flags) == EDataValidationResult::Invalid)
			{
				FDataValidationHelper::AddPrefixAndAppendIssues(InvalidCallbackEffectPrefix, Context, EffectContext);

				Result = EDataValidationResult::Invalid;
			}

			TSet<FUseIdInfo> EffectUseIds = BonusEffect->GetUseIds();

			TSet<FEffectId> UsedBeforeSavingIds;

			for (const FUseIdInfo& UseIdInfo : EffectUseIds)
			{
				if ((UseIdInfo.bUseLocalValue && !StageSaveIds.Contains(UseIdInfo.UseId)) ||
					(!UseIdInfo.bUseLocalValue && !SaveIds.Contains(UseIdInfo.UseId)))
				{
					UsedBeforeSavingIds.Add(UseIdInfo.UseId);
				}
			}

			if (!UsedBeforeSavingIds.IsEmpty() && !BonusEffect->SavesBeforeUsingIds(UsedBeforeSavingIds))
			{
				Context.AddError(FText::FromString(TEXT("A CallbackEffect uses an ID before it is saved")));

				Result = EDataValidationResult::Invalid;
			}

			StageUseIds.Append(EffectUseIds);
			StageSaveIds.Append(BonusEffect->GetSaveIds());
		}
	}

	SaveIds.Append(StageSaveIds);
	UseIds.Append(StageUseIds);

	return Result;
}
#endif

UEffect* UMultiStageBonusEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
	UMultiStageBonusEffect* NewEffect = DuplicateBaseEffect<UMultiStageBonusEffect>(Outer, Name);

	for (const EEffectStage Stage : TEnumRange<EEffectStage>())
	{
		for (auto EffectIt = Effects.CreateConstStageIterator(Stage); EffectIt; ++EffectIt)
		{
			NewEffect->Effects.AddToStage(*UEffect::DuplicateEffect(**EffectIt, Outer, *this, *NewEffect), Stage);
		}
	}

	NewEffect->BonusEffectIds = BonusEffectIds;

	return NewEffect;
}

void UMultiStageBonusEffect::SetBonusEffectId(const FEffectId Id)
{
	for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
	{
		(*EffectIt)->SetId(Id);
	}
}

bool UMultiStageBonusEffect::AddBonusEffect(const UMultiStageBonusEffect& Other, FEffectId& InOutEffectId)
{
	check(Other.BonusEffectIds.Num() <= 1);

	// Check if the ID is already set
	if (InOutEffectId == FEffectId::NoId)
	{
		// Copy the multistage bonus effect
		UMultiStageBonusEffect* OtherCopy = UEffect::DuplicateEffect(Other, GetOuter());

		InOutEffectId = BonusEffectIds.GetNextValidId();

		BonusEffectIds.Add(InOutEffectId);

		OtherCopy->SetBonusEffectId(InOutEffectId);

		TSet<FEffectId> SaveIds = GetSaveIds();
		TSet<FEffectId> OtherSaveIds = Other.GetSaveIds();
		TSet<FEffectId> OverlappingIds = SaveIds.Intersect(OtherSaveIds);

		if (!OverlappingIds.IsEmpty())
		{
			TSet<FEffectId> ValidSaveIds = FEffectIdSet::GetValidIds(SaveIds.Union(OtherSaveIds), OverlappingIds.Num());

			// Resolve any overlapping effect IDs
			OtherCopy->SwapEffectIds(OverlappingIds, ValidSaveIds);
		}

		Effects.Append(OtherCopy->Effects);

		return true;
	}
	else
	{
		for (auto EffectsIt = Effects.CreateIterator(); EffectsIt; ++EffectsIt)
		{
			UBonusEffect* Effect = *EffectsIt;

			if (Effect->IsIdEqualTo(InOutEffectId))
			{
				Effect->IncrementStacks();
			}
		}

		return false;
	}
}

void UMultiStageBonusEffect::RemoveBonusEffects(const FEffectId BonusEffectId)
{
	BonusEffectIds.Remove(BonusEffectId);

	for (auto EffectsIt = Effects.CreateIterator(); EffectsIt; ++EffectsIt)
	{
		UBonusEffect* Effect = *EffectsIt;

		if (Effect->IsIdEqualTo(BonusEffectId))
		{
			EffectsIt.RemoveCurrent();
		}				
	}
}

int32 UMultiStageBonusEffect::GetNumBonusEffects() const
{
	return BonusEffectIds.Num();
}

bool UMultiStageBonusEffect::HasEffects() const
{
	return !Effects.IsEmpty();
}

TSet<FEffectId> UMultiStageBonusEffect::GetSaveIds() const
{
	TSet<FEffectId> SaveIds;

	for (auto EffectIt = Effects.CreateConstIterator(); EffectIt; ++EffectIt)
	{
		SaveIds.Append((*EffectIt)->GetSaveIds());
	}

	return SaveIds;
}

void UMultiStageBonusEffect::SwapEffectIds(const TSet<FEffectId>& OldIds, const TSet<FEffectId>& NewIds)
{
	const TMap<FEffectId, FEffectId> IdMap = FEffectIdSet::GetIdSwapMap(OldIds, NewIds);

	for (auto EffectIt = Effects.CreateIterator(); EffectIt; ++EffectIt)
	{
		(*EffectIt)->SwapIds(IdMap);
	}
}
