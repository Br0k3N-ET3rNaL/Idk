// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/BonusEffect.h"

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectSavedValues.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/EffectSystem/SingleStageEffectGroup.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>

const FString UBonusEffect::EffectClassName = TEXT("BonusEffect");

UEffect* UBonusEffect::DuplicateEffectInternal(UObject* Outer, const FName& Name) const
{
    UBonusEffect* NewEffect = DuplicateBaseEffect<UBonusEffect>(Outer, Name);

    DeepCopyEffectGroup(Outer, NewEffect);

    NewEffect->Id = Id;
    NewEffect->Stacks = Stacks;

    return NewEffect;
}

void UBonusEffect::ApplyEffect(FApplyEffectParams& Params) const
{
    Params.CurrentSavedValues->SaveValue(FEffectId::StackId, Stacks);

    Super::ApplyEffect(Params);
}

void UBonusEffect::SetId(const FEffectId InId)
{
    Id = InId;
}

FEffectId UBonusEffect::GetId() const
{
    return Id;
}

bool UBonusEffect::IsIdEqualTo(const FEffectId InId) const
{
    return Id == InId;
}

void UBonusEffect::IncrementStacks()
{
    ++Stacks;
}
