// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/Components/StatusEffectComponent.h"

#include "Idk/EffectSystem/StatusEffectInternal.h"
#include "Idk/UI/StatusEffectBarWidget.h"
#include "Idk/UI/StatusEffectWidget.h"
#include <Containers/Map.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <TimerManager.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

UStatusEffectInstance& UStatusEffectInstance::Init(const UStatusEffectInternal& InStatusEffect, UAttributeSystemComponent& InAttributeSystem, FTimerManager& InTimerManager)
{
	StatusEffect = &InStatusEffect;
	AttributeSystem = &InAttributeSystem;
	TimerManager = &InTimerManager;

	UpdateDelegate = FTimerDelegate::CreateUObject(this, &UStatusEffectInstance::Update);

	return *this;
}

void UStatusEffectInstance::Apply()
{
	StatusEffect->ApplyPersistentEffects(AttributeSystem);

	UpdateStacksDelegate.Execute(Stacks);

	DurationRemaining = StatusEffect->Duration;

	if (IsPermanent())
	{
		UpdateTimeRemainingDelegate.Execute(1.0);
	}
	else
	{
		TimerManager->SetTimer(Timer, UpdateDelegate, FMath::Min(StatusEffect->Duration, UStatusEffectComponent::TimerInterval), true);

		UpdateTimeRemainingDelegate.Execute(0.0);
	}
}

void UStatusEffectInstance::Reapply()
{
	// Check if another stack can be added
	if (StatusEffect->MaxStacks > 1 && Stacks < StatusEffect->MaxStacks)
	{
		// Add another stack
		Stacks++;

		StatusEffect->ApplyPersistentEffects(AttributeSystem);

		UpdateStacksDelegate.Execute(Stacks);

		if (!StatusEffect->bRefreshable && !IsPermanent())
		{
			// Add timer for new stack
			StackTimers.Enqueue(FTimerHandle());
			TimerManager->SetTimer(*StackTimers.Peek(), StatusEffect->Duration, false);
		}
	}

	// Check if status effect can be refreshed
	if (StatusEffect->bRefreshable && !IsPermanent())
	{
		// Refresh duration
		DurationRemaining = StatusEffect->Duration;
		TimerManager->SetTimer(Timer, UpdateDelegate, FMath::Min(StatusEffect->Duration, UStatusEffectComponent::TimerInterval), true);

		UpdateTimeRemainingDelegate.Execute(1.0);
	}
}

void UStatusEffectInstance::Update()
{
	StatusEffect->ActivateEffect(AttributeSystem, Stacks, UStatusEffectComponent::TimerInterval);

	DurationRemaining -= UStatusEffectComponent::TimerInterval;

	// Check if the status effect has expired
	if (DurationRemaining <= 0.0)
	{
		// Check if only one stack should be removed
		if (Stacks > 1 && !StatusEffect->bRefreshable)
		{
			FTimerHandle NextStackTimer;

			// Remove a stack
			Stacks--;

			// Update the main timer from the oldest stack
			StackTimers.Dequeue(NextStackTimer);
			DurationRemaining = TimerManager->GetTimerRemaining(NextStackTimer);
			TimerManager->ClearTimer(NextStackTimer);

			double FirstDelay = FMath::Fmod(DurationRemaining, UStatusEffectComponent::TimerInterval);

			TimerManager->SetTimer(Timer, UpdateDelegate, UStatusEffectComponent::TimerInterval, true, FirstDelay);

			UpdateTimeRemainingDelegate.Execute(DurationRemaining / StatusEffect->Duration);
			UpdateStacksDelegate.Execute(Stacks);

			if (StatusEffect->HasPersistentEffects())
			{
				StatusEffect->RemoveAttributeAlteringEffects(AttributeSystem, 1);
			}
		}
		else
		{
			// Remove the status effect
			DurationRemaining = 0.0;
			TimerManager->ClearTimer(Timer);
			RemoveSelfDelegate.Execute();

			if (StatusEffect->HasPersistentEffects())
			{
				StatusEffect->RemoveAttributeAlteringEffects(AttributeSystem, Stacks);
			}
		}
	}
	else
	{
		UpdateTimeRemainingDelegate.Execute(DurationRemaining / StatusEffect->Duration);
	}
	
}

void UStatusEffectInstance::Remove()
{
	if (StatusEffect->HasPersistentEffects())
	{
		StatusEffect->RemoveAttributeAlteringEffects(AttributeSystem, Stacks);
	}

	Stop();

	RemoveSelfDelegate.Execute();
}

void UStatusEffectInstance::Stop()
{
	FTimerHandle StackTimer;

	while (StackTimers.Dequeue(StackTimer))
	{
		TimerManager->ClearTimer(StackTimer);
	}

	TimerManager->ClearTimer(Timer);

	UpdateDelegate.Unbind();
}

bool UStatusEffectInstance::IsPermanent() const
{
	return StatusEffect->Duration == UStatusEffectInternal::PermanentDuration;
}

uint8 UStatusEffectInstance::GetStacks() const
{
	return Stacks;
}

UStatusEffectComponent::UStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusEffectComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	// Stop all status effects
	for (TPair<FName, UStatusEffectInstance*>& Pair : StatusEffects)
	{
		Pair.Value->Stop();
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UStatusEffectComponent::SetAttributeSystem(UAttributeSystemComponent& InAttributeSystem)
{
	AttributeSystem = &InAttributeSystem;
}

void UStatusEffectComponent::SetStatusEffectBarWidget(UStatusEffectBarWidget& InStatusEffectBarWidget)
{
	StatusEffectBarWidget = &InStatusEffectBarWidget;
}

void UStatusEffectComponent::ApplyStatusEffect(const UStatusEffectInternal& StatusEffect)
{
	const FName& StatusName = StatusEffect.GetName();

	if (UStatusEffectInstance** StatusEffectInstance = StatusEffects.Find(StatusName))
	{
		(*StatusEffectInstance)->Reapply();
	}
	else
	{
		// Create an instance from the status effect
		UStatusEffectInstance* NewStatusEffectInstance = NewObject<UStatusEffectInstance>(this);

		NewStatusEffectInstance->Init(StatusEffect, *AttributeSystem, GetWorld()->GetTimerManager());

		UStatusEffectWidget& StatusEffectWidget = StatusEffectBarWidget->AddStatusEffect();

		StatusEffectWidget.Init(StatusEffect.GetDisplayInfo());

		// Bind delegates for status effect instance
		NewStatusEffectInstance->UpdateStacksDelegate.BindUObject(&StatusEffectWidget, &UStatusEffectWidget::UpdateStacks);
		NewStatusEffectInstance->UpdateTimeRemainingDelegate.BindUObject(&StatusEffectWidget, &UStatusEffectWidget::UpdateTimeRemaining);
		NewStatusEffectInstance->RemoveSelfDelegate.BindUObject(this, &UStatusEffectComponent::RemoveStatusEffectInternal, StatusName, &StatusEffectWidget);

		StatusEffects.Add(StatusName, NewStatusEffectInstance);

		NewStatusEffectInstance->Apply();
	}
}

bool UStatusEffectComponent::HasStatusEffect(const FName& StatusName) const
{
	return StatusEffects.Contains(StatusName);
}

uint8 UStatusEffectComponent::GetStatusEffectStacks(const FName& StatusName) const
{
	check(HasStatusEffect(StatusName));

	return (*StatusEffects.Find(StatusName))->GetStacks();
}

uint8 UStatusEffectComponent::RemoveStatusEffect(const FName& StatusName)
{
	uint8 Stacks = 0;

	if (UStatusEffectInstance** StatusEffect = StatusEffects.Find(StatusName))
	{
		Stacks = (*StatusEffect)->GetStacks();

		(*StatusEffect)->Remove();
	}

	return Stacks;
}

uint8 UStatusEffectComponent::ClearStatusEffects()
{
	uint8 TotalStacks = 0;

	// Stop all status effects
	for (TPair<FName, UStatusEffectInstance*>& Pair : StatusEffects)
	{
		TotalStacks += Pair.Value->GetStacks();

		Pair.Value->Stop();
	}

	// Remove all status effect next tick
	GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
		{
			StatusEffects.Empty();
			StatusEffectBarWidget->ClearStatusEffects();
		});

	return TotalStacks;
}

void UStatusEffectComponent::RemoveStatusEffectInternal(const FName StatusName, UStatusEffectWidget* StatusEffectWidget)
{
	check(StatusEffectWidget);

	StatusEffects.Remove(StatusName);
	StatusEffectBarWidget->RemoveStatusEffect(*StatusEffectWidget);
}
