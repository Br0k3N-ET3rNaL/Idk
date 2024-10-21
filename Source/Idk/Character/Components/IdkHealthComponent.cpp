// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/Components/IdkHealthComponent.h"

#include "Idk/UI/HealthBarWidget.h"
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>

UIdkHealthComponent::UIdkHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

double UIdkHealthComponent::GetHealth() const
{
	return Health;
}

double UIdkHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

double UIdkHealthComponent::GetHealthPercent() const
{
	check(MaxHealth > 0.0);

	return Health / MaxHealth;
}

void UIdkHealthComponent::SetMaxHealth(const double NewMaxHealth, bool bHeal)
{
	MaxHealth = NewMaxHealth;

	if (bHeal)
	{
		Health = MaxHealth;
	}

	if (HealthBarWidget)
	{
		UpdateWidget();
	}
}

void UIdkHealthComponent::SetHealthBarWidget(UHealthBarWidget& InHealthBarWidget)
{
	HealthBarWidget = &InHealthBarWidget;

	UpdateWidget();
}

double UIdkHealthComponent::TakeDamage(const double Damage)
{
	const double DamageDone = FMath::Min(Damage, Health);

	Health -= DamageDone;

	UpdateWidget();

	return DamageDone;
}

double UIdkHealthComponent::Heal(const double HealAmount)
{
	const double AmountHealed = FMath::Min(HealAmount, MaxHealth - Health);

	Health += AmountHealed;

	UpdateWidget();

	return AmountHealed;
}

void UIdkHealthComponent::RestoreSavedHealth(const double SavedHealth)
{
	check(SavedHealth <= MaxHealth);

	Health = SavedHealth;

	UpdateWidget();
}

void UIdkHealthComponent::UpdateWidget()
{
	HealthBarWidget->SetHealthPercent(GetHealthPercent());
}
