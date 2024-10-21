// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/AbilitySwapWidget.h"

#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/UI/CommonButton.h"
#include "Idk/UI/GenericDisplayWidget.h"
#include <Misc/AssertionMacros.h>

void UAbilitySwapWidget::SetAbilities(const FAbilityInfo& NewAbility, const FAbilityInfo* Ability1, const FAbilityInfo* Ability2)
{
	NewAbilityDisplay->Init(NewAbility.GetDisplayInfo());

	if (Ability1)
	{
		Ability1Button->SetText(SwapAbilityButtonText);

		Ability1Display->Init(Ability1->GetDisplayInfo());
	}
	else
	{
		Ability1Button->SetText(SetAbilityButtonText);
	}

	if (Ability2)
	{
		Ability2Button->SetText(SwapAbilityButtonText);

		Ability2Display->Init(Ability2->GetDisplayInfo());
	}
	else
	{
		Ability2Button->SetText(SetAbilityButtonText);
	}
}

void UAbilitySwapWidget::NativeOnInitialized()
{
	DiscardButton->OnClicked().AddUObject(this, &UAbilitySwapWidget::OnDiscardButtonClicked);
	Ability1Button->OnClicked().AddUObject(this, &UAbilitySwapWidget::OnAbility1ButtonClicked);
	Ability2Button->OnClicked().AddUObject(this, &UAbilitySwapWidget::OnAbility2ButtonClicked);
}

void UAbilitySwapWidget::OnDiscardButtonClicked() const
{
	check(DiscardAbilityDelegate.IsBound());

	DiscardAbilityDelegate.Execute();
}

void UAbilitySwapWidget::OnAbility1ButtonClicked() const
{
	check(SetAbilityDelegate.IsBound());

	SetAbilityDelegate.Execute(0);
}

void UAbilitySwapWidget::OnAbility2ButtonClicked() const
{
	check(SetAbilityDelegate.IsBound());

	SetAbilityDelegate.Execute(1);
}
