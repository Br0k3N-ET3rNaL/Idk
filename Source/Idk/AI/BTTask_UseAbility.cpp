// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTTask_UseAbility.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BTTaskNode.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTTask_UseAbility::UBTTask_UseAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("Use Ability");
	INIT_TASK_NODE_NOTIFY_FLAGS();

	IsUsingAbility.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_UseAbility, IsUsingAbility));

	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_UseAbility, Target), AActor::StaticClass());
}

FString UBTTask_UseAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Trigger AI ability %i"),
		*Super::GetStaticDescription(), AbilityIndex);
}

EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	check(Blackboard);

	AIdkEnemyCharacter* Enemy = CastChecked<AIdkEnemyCharacter>(AIController->GetPawn());

	AActor* TargetActor = CastChecked<AActor>(Blackboard->GetValueAsObject(Target.SelectedKeyName));

	check(!Blackboard->GetValueAsBool(IsUsingAbility.SelectedKeyName));

	Enemy->ActivateAbility(TargetActor, AbilityIndex);

	return EBTNodeResult::InProgress;
}

void UBTTask_UseAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		
		if (!Blackboard->GetValueAsBool(IsUsingAbility.SelectedKeyName))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
