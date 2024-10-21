// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/PedestalSelector.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/OrientToPlayerComponent.h"
#include "Idk/GameSystems/Pedestal.h"
#include "Idk/UI/PedestalDisplayInfo.h"
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

APedestalSelector::APedestalSelector()
{
	PrimaryActorTick.bCanEverTick = false;

	PedestalDisplayHeight = AIdkPlayerCharacter::GetInteractionHeight();

	// Create the component that will ensure that the widget always faces the player
	UOrientToPlayerComponent* OrientToPlayerComp =
		CreateDefaultSubobject<UOrientToPlayerComponent>(TEXT("OrientToPlayerComp"));
	OrientToPlayerComp->Init(false);

	RootComponent = OrientToPlayerComp;
}

#if WITH_EDITOR
EDataValidationResult APedestalSelector::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PedestalClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Pedestal class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void APedestalSelector::Destroyed()
{
	for (uint8 i = 0; i < Pedestals.Num(); ++i)
	{
		if (IsValid(Pedestals[i]))
		{
			Pedestals[i]->Destroy();
			Pedestals[i] = nullptr;
		}
	}
}

void APedestalSelector::Init(const TArray<FPedestalDisplayInfo>& PedestalDisplayInfo)
{
	// Y coordinate of the leftmost pedestal
	const double LeftMostY = ((NumPedestals - 1) / 2.0) * PedestalSpacing;

	UWorld* World = GetWorld();

	check(!PedestalClass.IsNull());
	check(PedestalDisplayInfo.Num() == NumPedestals);

	// Create and initialize each pedestal
	for (uint8 i = 0; i < NumPedestals; ++i)
	{
		APedestal* Pedestal = World->SpawnActor<APedestal>(PedestalClass.LoadSynchronous());
		Pedestal->GetRootComponent()->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Pedestal->SetActorRelativeLocation(FVector(0.0, LeftMostY - (i * PedestalSpacing), PedestalDisplayHeight));

		Pedestal->Init(PedestalDisplayInfo[i]);

		Pedestal->PedestalSelectedDelegate.BindUObject(this, &APedestalSelector::OnPedestalSelected, i);

		Pedestals.Add(Pedestal);
	}
}

uint8 APedestalSelector::GetNumPedestals() const
{
	return NumPedestals;
}

void APedestalSelector::OnPedestalSelected(const uint8 Index)
{
	PedestalSelectedDelegate.Execute(Index);
}
