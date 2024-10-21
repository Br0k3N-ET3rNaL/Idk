// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/CurseRoomManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemGenerator.h"
#include "Idk/GameSystems/PedestalSelector.h"
#include "Idk/GameSystems/RewardPedestalSelector.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

ACurseRoomManager::ACurseRoomManager()
{
	bHandleCompletion = true;

	SkipCurseDisplayInfo = FGenericDisplayInfo(TEXT("Skip Curse"), TEXT("Avoid the curses, but take 20 damage."));

	SkipCurseEffect = &FSimpleEffectBuilder::BeginDefault(this, TEXT("SkipCurseEffect"))
		.InitDamage(20.0)
		.Complete();
}

#if WITH_EDITOR
EDataValidationResult ACurseRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PedestalSelectorClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ACurseRoomManager: Pedestal selector class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	FDataValidationContext EffectContext;

	if (SkipCurseEffect && SkipCurseEffect->IsEffectValid(EffectContext) == EDataValidationResult::Invalid)
	{
		const FText InvalidEffectPrefix = FText::FromString(TEXT("ACurseRoomManager: Skip curse effect was invalid: "));

		FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ACurseRoomManager::Destroyed()
{
	if (PedestalSelector)
	{
		PedestalSelector->Destroy();
	}

	Super::Destroyed();
}

void ACurseRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (PedestalSelector)
	{
		PedestalSelector->Destroy();
	}
}

void ACurseRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	check(!PedestalSelectorClass.IsNull());

	PedestalSelector = GetWorld()->SpawnActor<ARewardPedestalSelector>(PedestalSelectorClass.LoadSynchronous());
	PedestalSelector->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	const UIdkGameInstance* GameInstance = UIdkGameInstance::GetGameInstance(GetWorld());
	UItemGenerator& ItemGenerator = GameInstance->GetItemGenerator();

	ItemGenerator.GenerateCursedItems(Rng->GenerateSeed(),
		PedestalSelector->GetNumPedestals() - 1, GeneratedItems, RemoveItemDelegates);

	TArray<FPedestalDisplayInfo> PedestalDisplayInfo;

	// Add the option to skip the cursed items.
	PedestalDisplayInfo.Emplace(SkipCurseDisplayInfo);

	for (const UItem* Item : GeneratedItems)
	{
		PedestalDisplayInfo.Emplace(Item->GetDisplayInfo());
	}

	PedestalSelector->Init(PedestalDisplayInfo);
	PedestalSelector->PedestalSelectedDelegate.BindUObject(this, &ACurseRoomManager::OnPedestalSelected);
}

void ACurseRoomManager::OnPedestalSelected(const uint8 Index)
{
	if (Index == SkipCursePedestalIndex)
	{
		FApplyEffectParams Params(nullptr, Player->GetAttributeSystem());

		SkipCurseEffect->ApplyEffect(Params);
	}
	else
	{
		RemoveItemDelegates[Index - 1].ExecuteIfBound();

		Player->AddItemToInventory(*GeneratedItems[Index - 1]);
	}

	Complete();
}
