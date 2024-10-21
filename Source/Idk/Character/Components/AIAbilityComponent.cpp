// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/AIAbilityComponent.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Delegates/DelegateSignatureImpl.inl>
#include <GameFramework/Actor.h>
#include <Idk/Character/IdkCharacter.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>

void UAIAbilityComponent::EndAbility()
{
	Super::EndAbility();

	SetOnCooldownDelegate.ExecuteIfBound(true);

	if (AbilityInfo->HasCooldown())
	{
		if (!AbilityInfo->IsCooldownInfinite())
		{
			GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UAIAbilityComponent::OnCooldownEnd, AbilityInfo->GetCooldown(), false);

			check(CooldownTimer.IsValid());
			check(GetWorld()->GetTimerManager().IsTimerActive(CooldownTimer));
		}

		OnAbilityEndDelegate.ExecuteIfBound();
	}
	else
	{
		OnAbilityEndDelegate.ExecuteIfBound();

		OnCooldownEnd();
	}
}

void UAIAbilityComponent::SetTarget(const AActor* InTarget)
{
	Target = InTarget;
}

FVector UAIAbilityComponent::GetPosInRangeOfTarget(const AActor* InTarget) const
{
	const FAbilityTargetingInfo& TargetingInfo = AbilityInfo->GetTargetingInfo();

	if (!TargetingInfo.HasInfiniteRange())
	{
		const FVector2D ToTarget = FVector2D(InTarget->GetActorLocation() - GetComponentLocation());

		// Distance to target
		const double Distance = ToTarget.Length();

		double Range = TargetingInfo.GetRange();

		// Adjust range to get a position where the target is in the middle of the area
		switch (TargetingInfo.GetAreaType())
		{
			case EAbilityAreaType::Cone:
				Range += TargetingInfo.GetRadius() / 2.0;
				break;
			case EAbilityAreaType::Rectangle:
				Range += TargetingInfo.GetLength() / 2.0;
				break;
			default:
				break;
		}

		if (Distance > Range && !FMath::IsNearlyEqual(Distance, Range, 0.9))
		{
			const FVector2D ToTargetInRange = ToTarget.GetSafeNormal() * Range;
			const FVector InRange = GetComponentLocation() + FVector(ToTarget - ToTargetInRange, 0.0);

			return InRange;
		}
	}

	return Owner->GetActorLocation();
}

bool UAIAbilityComponent::IsInRangeOfTarget(const AActor* InTarget) const
{
	const FAbilityTargetingInfo& TargetingInfo = AbilityInfo->GetTargetingInfo();

	if (!TargetingInfo.HasInfiniteRange())
	{
		const double Distance = FVector::Dist2D(InTarget->GetActorLocation(), GetComponentLocation());

		double Range = TargetingInfo.GetRange();

		switch (TargetingInfo.GetAreaType())
		{
			case EAbilityAreaType::Circle:
			case EAbilityAreaType::Cone:
				Range += TargetingInfo.GetRadius();
				break;
			case EAbilityAreaType::Rectangle:
				Range += TargetingInfo.GetLength();
				break;
			default:
				break;
		}

		return Distance < Range || FMath::IsNearlyEqual(Distance, Range, 0.9);
	}

	return true;
}

void UAIAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = CastChecked<AIdkEnemyCharacter>(GetOwner());
}

FVector UAIAbilityComponent::GetRangedTargetLocation() const
{
	return Target->GetActorLocation();
}

TSubclassOf<AIdkCharacter> UAIAbilityComponent::GetCollisionClassFilter() const
{
	return AIdkPlayerCharacter::StaticClass();
}

FVector UAIAbilityComponent::GetAimLocation() const
{
	return Target->GetActorLocation();
}

void UAIAbilityComponent::OnCooldownEnd()
{
	bOnCooldown = false;

	SetOnCooldownDelegate.ExecuteIfBound(false);
}
