// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/AbilityIndicatorComponent.h"

#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/IdkGameInstance.h"
#include <HAL/Platform.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>
#include <Math/MathFwd.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>

const FName UAbilityIndicatorComponent::IndicatorGreyPercent = TEXT("GreyPercent");
const FName UAbilityIndicatorComponent::IndicatorIsCircle = TEXT("bCircle");
const FName UAbilityIndicatorComponent::IndicatorIsCone = TEXT("bCone");
const FName UAbilityIndicatorComponent::IndicatorHalfAngle = TEXT("HalfAngle");

UAbilityIndicatorComponent::UAbilityIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	DecalSize = FVector(200.0, 200.0, 200.0);
	SetRelativeRotation(FRotator(90.0, 0.0, 0.0));
}

void UAbilityIndicatorComponent::UpdateIndicator()
{
	SetWorldLocation(GetAimLocationDelegate.Execute());
}

void UAbilityIndicatorComponent::SetupIndicator(const FAbilityTargetingInfo& AbilityTargetingInfo)
{
	TargetingInfo = &AbilityTargetingInfo;

	switch (AbilityTargetingInfo.GetAreaType())
	{
		case EAbilityAreaType::Circle:
			SetupCircleIndicator(AbilityTargetingInfo.GetRadius());
			break;

		case EAbilityAreaType::Cone:
			SetupConeIndicator(AbilityTargetingInfo.GetRadius(), AbilityTargetingInfo.GetHalfAngle());
			break;

		case EAbilityAreaType::Rectangle:
			SetupRectangleIndicator(AbilityTargetingInfo.GetWidth(), AbilityTargetingInfo.GetLength());
			break;

		default:
			break;
	}
}

void UAbilityIndicatorComponent::SetGreyedOut(const bool bGreyedOut)
{
	DynamicIndicatorMaterial->SetScalarParameterValue(IndicatorGreyPercent, (bGreyedOut) ? 1.f : 0.f);
}

void UAbilityIndicatorComponent::ApplyOffset()
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

void UAbilityIndicatorComponent::SetupCircleIndicator(const double Radius)
{
	DecalSize.Y = Radius;
	DecalSize.Z = Radius;

	DynamicIndicatorMaterial = UMaterialInstanceDynamic::Create(&GetIndicatorMaterial(GetWorld()), this);
	DynamicIndicatorMaterial->SetScalarParameterValue(IndicatorIsCircle, 1.f);
	DecalMaterial = DynamicIndicatorMaterial;
}

void UAbilityIndicatorComponent::SetupConeIndicator(const double Radius, const double HalfAngle)
{
	DecalSize.Y = Radius;
	DecalSize.Z = Radius;

	DynamicIndicatorMaterial = UMaterialInstanceDynamic::Create(&GetIndicatorMaterial(GetWorld()), this);
	DynamicIndicatorMaterial->SetScalarParameterValue(IndicatorIsCone, 1.f);
	DynamicIndicatorMaterial->SetScalarParameterValue(IndicatorHalfAngle, HalfAngle);
	DecalMaterial = DynamicIndicatorMaterial;
}

void UAbilityIndicatorComponent::SetupRectangleIndicator(const double Width, const double Length)
{
	DecalSize.Y = Width / 2.0;
	DecalSize.Z = Length / 2.0;

	DynamicIndicatorMaterial = UMaterialInstanceDynamic::Create(&GetIndicatorMaterial(GetWorld()), this);
	DecalMaterial = DynamicIndicatorMaterial;
}

UMaterialInterface& UAbilityIndicatorComponent::GetIndicatorMaterial(const UObject* WorldContextObject)
{
	return UIdkGameInstance::GetGameInstance(WorldContextObject)->GetAbilityIndicatorMaterial();
}
