// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Enemy/IdkAIController.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/IdkGameInstance.h"
#include <AIController.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Pawn.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

const FName AIdkAIController::PlayerKey = TEXT("PlayerCharacter");

#if WITH_EDITOR
EDataValidationResult AIdkAIController::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (BehaviorTree.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Behavior tree is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AIdkAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	check(!BehaviorTree.IsNull());
	RunBehaviorTree(BehaviorTree.LoadSynchronous());

	UIdkGameInstance* GameInstance = UIdkGameInstance::GetGameInstance(this);

	check(GameInstance);

	if (AIdkPlayerCharacter* Player = GameInstance->GetPlayer())
	{
		SetPlayer(*Player);
	}
	else
	{
		GameInstance->OnPlayerSetDelegate.AddLambda([&](AIdkPlayerCharacter* Player)
			{
				check(Player);

				SetPlayer(*Player);
			});
	}
}

void AIdkAIController::OnUnPossess()
{
	Destroy();
}

void AIdkAIController::SetPlayer(AIdkPlayerCharacter& Player)
{
	SetFocus(&Player, EAIFocusPriority::Gameplay);
	Blackboard->SetValueAsObject(PlayerKey, &Player);
}
