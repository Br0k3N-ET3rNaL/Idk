// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTTask_SetPlanted.h"

#include "Idk/Character/SiegeEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BehaviorTreeTypes.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTTask_SetPlanted::UBTTask_SetPlanted(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("Set Planted");

	IsPlanted.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetPlanted, IsPlanted));
}

EBTNodeResult::Type UBTTask_SetPlanted::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (ASiegeEnemy* Enemy = Cast<ASiegeEnemy>(AIController->GetPawn()))
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		check(Blackboard);

		Blackboard->SetValueAsBool(IsPlanted.SelectedKeyName, bPlanted);

		Enemy->OnFinishedPlantingDelegate.BindUObject(this, &UBTTask_SetPlanted::OnFinishedPlanting, &OwnerComp);

		Enemy->SetPlanted(bPlanted);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_SetPlanted::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s the planted state."),
		*Super::GetStaticDescription(), (bPlanted) ? TEXT("Enter") : TEXT("Exit"));
}

void UBTTask_SetPlanted::OnFinishedPlanting(UBehaviorTreeComponent* OwnerComp)
{
	check(OwnerComp);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Type::Succeeded);
}
