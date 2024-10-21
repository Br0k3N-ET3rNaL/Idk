// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/AbilityGeneratorComponent.h"

#include "Idk/Character/PlayerSaveData.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/UI/AbilityChoiceWidget.h"
#include "Idk/UI/AbilitySwapWidget.h"
#include <Containers/Array.h>
#include <CoreGlobals.h>
#include <HAL/Platform.h>
#include <Logging/LogMacros.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <UObject/NameTypes.h>

UAbilityGeneratorComponent::UAbilityGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityGeneratorComponent::Init(const int32 Seed, const TArray<FAbilityInfo*>& InAbilityPool)
{
	check(InAbilityPool.Num() > 0);

	AbilityPool = &InAbilityPool;

	Rng = &UIdkRandomStream::CreateRandomStream(this, Seed);
}

void UAbilityGeneratorComponent::SetAbilityChoiceWidget(UAbilityChoiceWidget& InAbilityChoiceWidget)
{
	AbilityChoiceWidget = &InAbilityChoiceWidget;

	AbilityChoiceWidget->AbilityChosenDelegate.BindUObject(this, &UAbilityGeneratorComponent::OnNewAbilityChosen);
}

void UAbilityGeneratorComponent::SetAbilitySwapWidget(UAbilitySwapWidget& InAbilitySwapWidget)
{
	AbilitySwapWidget = &InAbilitySwapWidget;

	AbilitySwapWidget->DiscardAbilityDelegate.BindUObject(this, &UAbilityGeneratorComponent::DiscardNewAbility);
	AbilitySwapWidget->SetAbilityDelegate.BindUObject(this, &UAbilityGeneratorComponent::SwapNewAbility);
}

void UAbilityGeneratorComponent::GenerateAbilities(const int32 Num, const FAbilityInfo* InExistingAbility1, const FAbilityInfo* InExistingAbility2)
{
	//check(AbilityPool->Num() >= 3);
	check(Num > 0);
	check(AbilityChoiceWidget);
	check(ToggleAbilityChoiceWidgetDelegate.IsBound());
	check(ToggleAbilitySwapWidgetDelegate.IsBound());

	ExistingAbility1 = InExistingAbility1;
	ExistingAbility2 = InExistingAbility2;

	TArray<const FAbilityInfo*> AbilityInfo;

	int32 AbilitiesRemaining = AbilityPool->Num();

	if (ExistingAbility1)
	{
		--AbilitiesRemaining;
	}

	if (ExistingAbility2)
	{
		--AbilitiesRemaining;
	}

	// Remove later 
	if (AbilitiesRemaining < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough abilities remaining"));

		return;
	}

	const int32 NumToGenerate = FMath::Min(Num, AbilitiesRemaining);
	const int32 Max = AbilityPool->Num();

	TArray<int32> ValidIndices;

	for (int32 i = 0; i < Max; ++i)
	{
		if ((*AbilityPool)[i] != ExistingAbility1
			&& (*AbilityPool)[i] != ExistingAbility2)
		{
			ValidIndices.Add(i);
		}
	}

	for (int32 i = 0; i < NumToGenerate; ++i)
	{
		const int32 RandNum = Rng->RandRange(0, ValidIndices.Num() - 1);

		const int32 Index = ValidIndices[RandNum];

		GeneratedIndices.Add(Index);

		AbilityInfo.Add((*AbilityPool)[Index]);

		ValidIndices.RemoveAt(RandNum);
	}

	AbilityChoiceWidget->SetChoices(AbilityInfo);

	// Show ability choice widget
	ToggleAbilityChoiceWidgetDelegate.Execute();
}

void UAbilityGeneratorComponent::OnNewAbilityChosen(const int32 GeneratedAbilityIndex)
{
	check(AddAbilityToPlayerDelegate.IsBound());

	GeneratedIndices.RangeCheck(GeneratedAbilityIndex);

	NewAbility = (*AbilityPool)[GeneratedIndices[GeneratedAbilityIndex]];

	check(NewAbility);
	check(NewAbility != ExistingAbility1 && NewAbility != ExistingAbility2);

	GeneratedIndices.Empty();

	AbilitySwapWidget->SetAbilities(*NewAbility, ExistingAbility1, ExistingAbility2);

	// Hide ability choice widget
	ToggleAbilityChoiceWidgetDelegate.Execute();

	// Show ability swap widget
	ToggleAbilitySwapWidgetDelegate.Execute();
}

void UAbilityGeneratorComponent::DiscardNewAbility()
{
	NewAbility = nullptr;
	ExistingAbility1 = nullptr;
	ExistingAbility2 = nullptr;

	ToggleAbilitySwapWidgetDelegate.Execute();
}

void UAbilityGeneratorComponent::SwapNewAbility(const int32 AbilityIndex)
{
	check(SaveData);
	check(AbilityIndex == 0 || AbilityIndex == 1);

	if (AbilityIndex == 0)
	{
		SaveData->SetAbility1Name((NewAbility->GetName()));
	}
	else
	{
		SaveData->SetAbility2Name((NewAbility->GetName()));
	}

	AddAbilityToPlayerDelegate.Execute(AbilityIndex, *NewAbility);

	NewAbility = nullptr;
	ExistingAbility1 = nullptr;
	ExistingAbility2 = nullptr;

	ToggleAbilitySwapWidgetDelegate.Execute();
}

void UAbilityGeneratorComponent::SetSaveData(FPlayerSaveData& InSaveData)
{
	SaveData = &InSaveData;
}

void UAbilityGeneratorComponent::RestoreSavedAbilities()
{
	check(SaveData);

	const FName& Ability1Name = SaveData->GetAbility1Name();
	const FName& Ability2Name = SaveData->GetAbility2Name();

	for (FAbilityInfo* AbilityInfo : *AbilityPool)
	{
		if (AbilityInfo->GetName().IsEqual(Ability1Name))
		{
			AddAbilityToPlayerDelegate.Execute(0, *AbilityInfo);
		}
		if (AbilityInfo->GetName().IsEqual(Ability2Name))
		{
			AddAbilityToPlayerDelegate.Execute(1, *AbilityInfo);
		}
	}
}
