// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/PlayerAbilityComponent.h"

#include "Idk/Character/Components/AbilityIndicatorComponent.h"
#include "Idk/Character/IdkCharacter.h"
#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/UI/AbilityWidget.h"
#include <Engine/EngineBaseTypes.h>
#include <Engine/HitResult.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>

const FName UPlayerAbilityComponent::TraceCollisionProfile = TEXT("Spectator");

UPlayerAbilityComponent::UPlayerAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = CastChecked<AIdkPlayerCharacter>(GetOwner());
}

void UPlayerAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bOnCooldown)
	{
		CooldownRemaining -= DeltaTime;

		if (CooldownRemaining <= 0.0)
		{
			CooldownRemaining = 0.0;
			bOnCooldown = false;

			if (CanShowIndicator())
			{
				IndicatorComp->SetGreyedOut(false);
			}

			PrimaryComponentTick.SetTickFunctionEnable(false);

			if (AbilityWidget)
			{
				AbilityWidget->EndCooldown();
			}
		}
		else if (AbilityWidget)
		{
			const double CooldownPercent = CooldownRemaining / AbilityInfo->GetCooldown();

			AbilityWidget->UpdateCooldown(CooldownRemaining, CooldownPercent);
		}
	}
}

void UPlayerAbilityComponent::SetAbility(const FAbilityInfo& Ability)
{
	Super::SetAbility(Ability);

	check(!Ability.IsCooldownInfinite());

	if (AbilityWidget)
	{
		AbilityWidget->Init(AbilityInfo->GetDisplayInfo());
	}

	TraceDistance = AbilityInfo->GetTargetingInfo().GetRange() / (UE_DOUBLE_SQRT_2 / 2.0);
}

bool UPlayerAbilityComponent::CanShowIndicator() const
{
	return Super::CanShowIndicator() && !bUsingAbility;
}

void UPlayerAbilityComponent::ShowIndicator()
{
	Super::ShowIndicator();

	IndicatorComp->SetGreyedOut(bOnCooldown);
}

void UPlayerAbilityComponent::StartAbility()
{
	if (AbilityWidget)
	{
		AbilityWidget->DisableAbility();
	}

	bUsingAbility = true;

	Super::StartAbility();
}

void UPlayerAbilityComponent::EndAbility()
{
	CooldownRemaining = AbilityInfo->GetCooldown();

	if (bTickIndicator)
	{
		bTickIndicator = false;
		IndicatorComp->SetVisibility(false);
	}
	else
	{
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}

	bUsingAbility = false;

	Super::EndAbility();
}

void UPlayerAbilityComponent::SetAbilityWidget(UAbilityWidget& InAbilityWidget)
{
	AbilityWidget = &InAbilityWidget;
}

bool UPlayerAbilityComponent::CanUseAbility() const
{
	return IsAbilitySet() && !bOnCooldown && !bUsingAbility;
}

bool UPlayerAbilityComponent::IsUsingAbility() const
{
	return bUsingAbility;
}

FVector UPlayerAbilityComponent::GetRangedTargetLocation() const
{
	if (IndicatorComp)
	{
		return IndicatorComp->GetComponentLocation();
	}
	
	return GetAimLocation();
}

TSubclassOf<AIdkCharacter> UPlayerAbilityComponent::GetCollisionClassFilter() const
{
	return AIdkEnemyCharacter::StaticClass();
}

FVector UPlayerAbilityComponent::GetAimLocation() const
{
	// Ability range
	const double Range = AbilityInfo->GetTargetingInfo().GetRange();

	// Start position of line trace
	const FVector Start = Player->GetCameraLocation();

	const FVector CameraForward = Player->GetCameraForwardVector();

	// End position of line trace
	const FVector End = Start + (CameraForward * TraceDistance);

	FHitResult HitResult;

	if (GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, TraceCollisionProfile))
	{
		const FVector2D HitPos2D = FVector2D(HitResult.Location);
		const FVector PlayerPos = Player->GetActorLocation();
		const FVector2D PlayerPos2D = FVector2D(PlayerPos);
		const double Distance = FVector2D::Distance(HitPos2D, PlayerPos2D);

		if (Distance <= Range)
		{
			return FVector(HitPos2D, PlayerPos.Z);
		}
		
		const FVector2D ToHitPos2D = HitPos2D - PlayerPos2D;

		return FVector((ToHitPos2D.GetSafeNormal() * Range) + PlayerPos2D, PlayerPos.Z);
	}
	
	return (Player->GetActorForwardVector() * Range) + Player->GetActorLocation();
}
