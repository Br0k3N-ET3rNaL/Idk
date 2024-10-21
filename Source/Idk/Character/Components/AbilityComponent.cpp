 // Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/AbilityComponent.h"

#include "Idk/Character/Components/AbilityCollisionComponent.h"
#include "Idk/Character/Components/AbilityIndicatorComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Components/SceneComponent.h>
#include <Containers/Array.h>
#include <Engine/CollisionProfile.h>
#include <Engine/EngineBaseTypes.h>
#include <Engine/EngineTypes.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <UObject/UObjectGlobals.h>

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTickIndicator)
	{
		IndicatorComp->UpdateIndicator();
	}
}

void UAbilityComponent::SetAbility(const FAbilityInfo& Ability)
{
	AbilityInfo = &Ability;

	if (AbilityInfo->GetTargetingInfo().HandlesCollision())
	{
		if (!CollisionComp)
		{
			CollisionComp = NewObject<UAbilityCollisionComponent>(this, TEXT("AbilityCollisionComp"));
			CollisionComp->RegisterComponent();
		}

		CollisionComp->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CollisionComp->SetupCollision(AbilityInfo->GetTargetingInfo());
		CollisionComp->ApplyOffset();

		if (!IndicatorComp)
		{
			IndicatorComp = NewObject<UAbilityIndicatorComponent>(this, TEXT("AbilityIndicatorComp"));
			IndicatorComp->RegisterComponent();
		}

		IndicatorComp->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, false));
		IndicatorComp->SetVisibility(false);
		IndicatorComp->SetupIndicator(AbilityInfo->GetTargetingInfo());
		IndicatorComp->ApplyOffset();

		IndicatorComp->GetAimLocationDelegate.Unbind();
		IndicatorComp->GetAimLocationDelegate.BindUObject(this, &UAbilityComponent::GetAimLocation);
	}
	else
	{
		if (CollisionComp)
		{
			CollisionComp->DestroyComponent();
			CollisionComp = nullptr;
		}

		if (IndicatorComp)
		{
			IndicatorComp->DestroyComponent();
			IndicatorComp = nullptr;
		}
	}
}

bool UAbilityComponent::CanShowIndicator() const
{
	if (!AbilityInfo)
	{
		return false;
	}

	return (AbilityInfo->GetTargetingInfo().GetAreaType() == EAbilityAreaType::Custom) ? false : true;
}

void UAbilityComponent::ShowIndicator()
{
	IndicatorComp->SetVisibility(true);

	if (AbilityInfo->GetTargetingInfo().UsesRangedTargeting())
	{
		bTickIndicator = true;

		PrimaryComponentTick.SetTickFunctionEnable(true);
	}
}

void UAbilityComponent::HideIndicator()
{
	IndicatorComp->SetVisibility(false);

	bTickIndicator = false;
}

void UAbilityComponent::StartAbility()
{
	check(AbilityInfo);

	InitialTargetLocation = GetTargetLocation();

	AbilityInfo->ActivateAbilityDelegate.Execute();
}

void UAbilityComponent::EndAbility()
{
	bOnCooldown = true;
}

void UAbilityComponent::SetAttributeSystem(UAttributeSystemComponent* InOwnersAttributeSystem)
{
	OwnersAttributeSystem = InOwnersAttributeSystem;
}

bool UAbilityComponent::IsAbilitySet() const
{
	return AbilityInfo != nullptr;
}

bool UAbilityComponent::IsOnCooldown() const
{
	return bOnCooldown;
}

bool UAbilityComponent::IsIndicatorVisible() const
{
	return IndicatorComp && IndicatorComp->IsVisible();
}

void UAbilityComponent::ApplyEffect()
{
	ApplyEffectToSelf();

	if (AbilityInfo->GetTargetingInfo().HandlesCollision())
	{
		ApplyEffectDeferred();
	}
}

void UAbilityComponent::ApplyEffectToSelf()
{
	ActivationLocation = GetTargetLocation();

	AbilityInfo->GetEffect().ApplyEffectsToSelf(*OwnersAttributeSystem, FVector2D(ActivationLocation));
}

void UAbilityComponent::ApplyEffectDeferred()
{
	if (AbilityInfo->GetTargetingInfo().UsesRangedActivation())
	{
		CollisionComp->SetWorldLocation(ActivationLocation);
	}

	TArray<UAttributeSystemComponent*> OverlappingAttributeSystems;

	CollisionComp->GetOverlappingAttributeSystems(OverlappingAttributeSystems, GetCollisionClassFilter());

	AbilityInfo->GetEffect().ApplyEffectsToTargets(*OwnersAttributeSystem, OverlappingAttributeSystems, FVector2D(CollisionComp->GetComponentLocation()), AbilityInfo->GetPerHitMultiplier());
}

void UAbilityComponent::ApplyEffectToTargetDeferred(UAttributeSystemComponent* TargetAttributeSystem)
{
	check(!AbilityInfo->GetTargetingInfo().HandlesCollision());

	AbilityInfo->GetEffect().ApplyEffectsToTarget(*OwnersAttributeSystem, *TargetAttributeSystem, FVector2D(GetTargetLocation()), AbilityInfo->GetPerHitMultiplier());
}

FVector UAbilityComponent::GetTargetLocation() const
{
	if (AbilityInfo->GetTargetingInfo().UsesRangedTargeting())
	{
		return GetRangedTargetLocation();
	}
	else
	{
		return GetComponentLocation();
	}
}

FVector UAbilityComponent::GetInitialTargetLocation() const
{
	return InitialTargetLocation;
}

const FAbilityInfo* UAbilityComponent::GetAbilityInfo() const
{
	return AbilityInfo;
}

void UAbilityComponent::DisableCollisions()
{
	if (CollisionComp)
	{
		CollisionComp->SetGenerateOverlapEvents(false);
		CollisionComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	}
}
