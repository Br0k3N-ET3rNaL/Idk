// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/RoomManagers/RestRoomManager.h"

#include "Idk/GameSystems/HealPickup.h"
#include <Engine/EngineTypes.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult ARestRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (HealPickupClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Heal pickup class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (HealingCurve.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Healing curve is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ARestRoomManager::Destroyed()
{
	if (HealPickup)
	{
		HealPickup->Destroy();
	}

	Super::Destroyed();
}

void ARestRoomManager::OnRoomActivated()
{
	Super::OnRoomActivated();

	check(!HealPickupClass.IsNull());
	check(!HealingCurve.IsNull());

	// Spawn the heal pickup
	HealPickup = GetWorld()->SpawnActor<AHealPickup>(HealPickupClass.LoadSynchronous());
	HealPickup->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	HealPickup->Init(HealingCurve.LoadSynchronous()->GetFloatValue(Level));
	HealPickup->OnPickedUpDelegate.BindUObject(this, &ARestRoomManager::Complete);
}

void ARestRoomManager::OnCompletion()
{
	Super::OnCompletion();

	if (HealPickup)
	{
		HealPickup->Destroy();
	}
}
