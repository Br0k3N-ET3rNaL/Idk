// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/SiegeEnemy.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include <Animation/AnimMontage.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/DataValidation.h>
#include <UObject/SoftObjectPtr.h>
#include <UObject/UObjectGlobals.h>

ASiegeEnemy::ASiegeEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ChargeTime = 0.1;

	AttributeSystemComponent->InitBaseHealthMult(1.2);
}

#if WITH_EDITOR
EDataValidationResult ASiegeEnemy::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (EnterPlantedMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ASiegeEnemy: Enter planted montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (ExitPlantedMontage.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ASiegeEnemy: Exit planted montage is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ASiegeEnemy::SetPlanted(const bool bInPlanted)
{
	bPlanted = bInPlanted;

	SetPlantedDelegate.Broadcast(bPlanted);

	TSoftObjectPtr<UAnimMontage>& PlantedMontage = (bPlanted) ? EnterPlantedMontage : ExitPlantedMontage;

	AnimInstance->Montage_Play(PlantedMontage.LoadSynchronous());

	const FRotator& NewRotationRate = (bPlanted) ? PlantedRotationRate : BaseRotationRate;

	GetCharacterMovement()->RotationRate = NewRotationRate;
}

void ASiegeEnemy::BeginPlay()
{
	Super::BeginPlay();

	BaseRotationRate = GetCharacterMovement()->RotationRate;
	PlantedRotationRate = BaseRotationRate * PlantedRotationMultiplier;
}

void ASiegeEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == EnterPlantedMontage.LoadSynchronous()
		|| Montage == ExitPlantedMontage.LoadSynchronous())
	{
		OnFinishedPlantingDelegate.ExecuteIfBound();
	}

	Super::OnMontageEnded(Montage, bInterrupted);
}
