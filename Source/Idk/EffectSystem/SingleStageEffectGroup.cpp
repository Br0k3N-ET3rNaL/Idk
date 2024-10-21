// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/SingleStageEffectGroup.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>
#include <Templates/Function.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

const FString USingleStageEffectGroup::EffectClassName = TEXT("EffectGroup");

#if WITH_EDITOR
EDataValidationResult USingleStageEffectGroup::IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags) const
{
    const EDataValidationResult SuperResult = Super::IsEffectValid(Context, Flags);
    EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

    TSet<FEffectId> SaveIds;
    TSet<FUseIdInfo> UseIds;

    const FText InvalidEffectPrefix = FText::FromString(TEXT("USingleStageEffectGroup: Sub-effect was invalid: "));

    for (const USingleStageEffect* Effect : Effects)
    {
        FDataValidationContext EffectContext;

        if (Effect->IsDataValid(EffectContext) == EDataValidationResult::Invalid)
        {
            FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

            Result = EDataValidationResult::Invalid;

            continue;
        }

        if (bSelfContained)
        {
            TSet<FUseIdInfo> EffectUseIds = Effect->GetUseIds();

            TSet<FEffectId> UsedBeforeSavingIds;

            for (const FUseIdInfo& UseIdInfo : EffectUseIds)
            {
                if (!UseIdInfo.bUseLocalValue)
                {
                    Context.AddError(FText::FromString(FString::Printf(
                        TEXT("USingleStageEffectGroup: bUseLocalValue for ID = %u was false, but effect is self contained, so it can only use local values"),
                        UseIdInfo.UseId)));

                    Result = EDataValidationResult::Invalid;
                }

                if (!SaveIds.Contains(UseIdInfo.UseId))
                {
                    UsedBeforeSavingIds.Add(UseIdInfo.UseId);
                }
            }

            if (!UsedBeforeSavingIds.IsEmpty() && !Effect->SavesBeforeUsingIds(UsedBeforeSavingIds))
            {
                Context.AddError(FText::FromString(TEXT("USingleStageEffectGroup: Sub-effect uses an ID before it is saved")));

                Result = EDataValidationResult::Invalid;
            }

            UseIds.Append(EffectUseIds);
            SaveIds.Append(Effect->GetSaveIds());
        }
    }

    return Result;
}
#endif

UEffect* USingleStageEffectGroup::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
    USingleStageEffectGroup* NewEffect = DuplicateBaseEffect<USingleStageEffectGroup>(Outer, Name);

    DeepCopyEffectGroup(Outer, NewEffect);

    return NewEffect;
}

void USingleStageEffectGroup::InitDescriptions(UAttributeSystemComponent& AttributeSystem)
{
    for (USingleStageEffect* Effect : Effects)
    {
        Effect->InitDescriptions(AttributeSystem);
    }
}

void USingleStageEffectGroup::ApplyEffect(FApplyEffectParams& Params) const
{
    for (const USingleStageEffect* Effect : Effects)
    {
        Effect->ApplyEffect(Params);
    }
}

TSet<FEffectId> USingleStageEffectGroup::GetSaveIds() const
{
    TSet<FEffectId> SaveIds;

    if (!bSelfContained)
    {
        for (const USingleStageEffect* Effect : Effects)
        {
            SaveIds.Append(Effect->GetSaveIds());
        }
    }

    return SaveIds;
}

TSet<FUseIdInfo> USingleStageEffectGroup::GetUseIds() const
{
    TSet<FUseIdInfo> UseIds;

    if (!bSelfContained)
    {
        for (const USingleStageEffect* Effect : Effects)
        {
            UseIds.Append(Effect->GetUseIds());
        }
    }

    return UseIds;
}

bool USingleStageEffectGroup::SavesBeforeUsingIds(const TSet<FEffectId>& Ids) const
{
    if (!bSelfContained)
    {
        TSet<FEffectId> SaveIds;
        TSet<FUseIdInfo> UseIds;

        for (const USingleStageEffect* Effect : Effects)
        {
            UseIds.Append(Effect->GetUseIds());

            for (const FUseIdInfo& UseIdInfo : UseIds)
            {
                if (Ids.Contains(UseIdInfo.UseId) && !SaveIds.Contains(UseIdInfo.UseId) && !Effect->SavesBeforeUsingIds(Ids))
                {
                    return false;
                }
            }

            SaveIds.Append(Effect->GetSaveIds());
        }
    }

    return true;
}

void USingleStageEffectGroup::SwapIds(const TMap<FEffectId, FEffectId>& IdMap)
{
    for (USingleStageEffect* Effect : Effects)
    {
        Effect->SwapIds(IdMap);
    }
}

void USingleStageEffectGroup::GetEffectsByType(const EEffectType EffectType, TArray<USimpleEffect*>& OutEffects)
{
    for (USingleStageEffect* Effect : Effects)
    {
        Effect->GetEffectsByType(EffectType, OutEffects);
    }
}

void USingleStageEffectGroup::GetTooltipParams(const FApplyEffectParams& EffectParams, TMap<FName, FText>& StatusEffectParams, TMap<EEffectType, double>& EffectValueParams) const
{
    for (auto EffectsIt = Effects.CreateConstIterator(); EffectsIt; ++EffectsIt)
    {
        (*EffectsIt)->GetTooltipParams(EffectParams, StatusEffectParams, EffectValueParams);
    }
}

void USingleStageEffectGroup::RemoveEffectsByUseId(const FEffectId UseId, TFunctionRef<void()> RemoveSelf)
{
    for (auto EffectsIt = Effects.CreateIterator(); EffectsIt; ++EffectsIt)
    {
        (*EffectsIt)->RemoveEffectsByUseId(UseId, [&]()
            {
                EffectsIt.RemoveCurrent();
            });
    }
}

void USingleStageEffectGroup::SetSelfContained()
{
    bSelfContained = true;
}

void USingleStageEffectGroup::DeepCopyEffectGroup(UObject* Outer, USingleStageEffectGroup* NewEffect) const
{
    for (const USingleStageEffect* Effect : Effects)
    {
        NewEffect->Effects.Add(UEffect::DuplicateEffect(*Effect, Outer, *this, *NewEffect));
    }

    NewEffect->bSelfContained = bSelfContained;
}
