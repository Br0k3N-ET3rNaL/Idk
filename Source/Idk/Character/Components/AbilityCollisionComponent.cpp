// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/AbilityCollisionComponent.h"

#include "Idk/Character/IdkCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Components/CapsuleComponent.h>
#include <Containers/Array.h>
#include <Containers/Set.h>
#include <Engine/CollisionProfile.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>

const FName UAbilityCollisionComponent::CollisionPreset = TEXT("OverlapOnlyPawn");

UAbilityCollisionComponent::UAbilityCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetGenerateOverlapEvents(true);
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void UAbilityCollisionComponent::SetupCollision(const FAbilityTargetingInfo& AbilityTargetingInfo)
{
	TargetingInfo = &AbilityTargetingInfo;

	switch (TargetingInfo->GetAreaType())
	{
		case EAbilityAreaType::Circle:
			SetBoxExtent(FVector(TargetingInfo->GetRadius(), TargetingInfo->GetRadius(), CollisionHeight));
			break;

		case EAbilityAreaType::Cone:
			SetBoxExtent(GetExtentForCone(TargetingInfo->GetRadius(), TargetingInfo->GetHalfAngle()));
			break;

		case EAbilityAreaType::Rectangle:
			SetBoxExtent(FVector(TargetingInfo->GetLength() / 2.0, TargetingInfo->GetWidth() / 2.0, CollisionHeight));
			ApplyOffset();
			break;

		default:
			break;
	}
}

void UAbilityCollisionComponent::GetOverlappingAttributeSystems(TArray<UAttributeSystemComponent*>& OutOverlappingCharacters, TSubclassOf<AIdkCharacter> ClassFilter)
{
	TArray<AActor*> OverlappingActors;

	// Enable collisions
	SetCollisionProfileName(CollisionPreset);

	GetOverlappingActors(OverlappingActors, ClassFilter);

	// Limit results based on area type
	switch (TargetingInfo->GetAreaType())
	{
		case EAbilityAreaType::Circle:
		{
			const FVector Center = GetComponentLocation();

			for (AActor* Actor : OverlappingActors)
			{
				if (AIdkCharacter* Character = Cast<AIdkCharacter>(Actor))
				{
					const FVector CharacterLocation = Character->GetActorLocation();
					const double CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

					const double Distance = FVector::Dist(Center, CharacterLocation);

					if (Distance - CapsuleRadius <= TargetingInfo->GetRadius())
					{
						OutOverlappingCharacters.AddUnique(Character->GetAttributeSystem());
					}
				}
			}
			break;
		}
		case EAbilityAreaType::Cone:
		{
			const FVector Center = GetComponentLocation();
			const FVector Forward = GetOwner()->GetActorForwardVector();

			for (AActor* Actor : OverlappingActors)
			{
				if (AIdkCharacter* Character = Cast<AIdkCharacter>(Actor))
				{
					const FVector CharacterLocation = Character->GetActorLocation();
					const double CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

					const double Distance = FVector::Dist(Center, CharacterLocation);

					if (Distance - CapsuleRadius <= TargetingInfo->GetRadius())
					{
						const FVector ToCharacter = CharacterLocation - Center;

						if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToCharacter))) <= TargetingInfo->GetHalfAngle())
						{
							OutOverlappingCharacters.AddUnique(Character->GetAttributeSystem());
						}
					}
				}
			}
			break;
		}
		case EAbilityAreaType::Rectangle:
		{
			for (AActor* Actor : OverlappingActors)
			{
				if (AIdkCharacter* Character = Cast<AIdkCharacter>(Actor))
				{
					OutOverlappingCharacters.AddUnique(Character->GetAttributeSystem());
				}
			}
		}
			break;
		default:
			break;
	}

	// Disable collisions
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void UAbilityCollisionComponent::ApplyOffset()
{
	if (TargetingInfo)
	{
		if (TargetingInfo->GetAreaType() == EAbilityAreaType::Rectangle)
		{
			SetRelativeLocation(FVector(TargetingInfo->GetLength() / 2.0, 0.0, 0.0));
		}
		else if (TargetingInfo->GetAreaType() == EAbilityAreaType::Cone)
		{
			SetRelativeLocation(FVector(TargetingInfo->GetRadius() / 2.0, 0.0, 0.0));
		}
	}
}

FVector UAbilityCollisionComponent::GetExtentForCone(const double Radius, const double HalfAngle)
{
	const double ConeWidth = FMath::Sin(FMath::DegreesToRadians(HalfAngle)) * Radius;
	
	return FVector(ConeWidth, Radius, CollisionHeight);
}
