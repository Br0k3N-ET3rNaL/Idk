// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTDecorator_IsActorValid.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

UBTDecorator_IsActorValid::UBTDecorator_IsActorValid(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Actor Valid");

	ActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsActorValid, ActorKey), AActor::StaticClass());
}

bool UBTDecorator_IsActorValid::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	if (UObject* Object = Blackboard->GetValueAsObject(ActorKey.SelectedKeyName))
	{
		if (Object->IsA<AActor>() && IsValid(Object))
		{
			return true;
		}
	}

	return false;
}

FString UBTDecorator_IsActorValid::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Check if %s is a valid actor"),
		*Super::GetStaticDescription(),
		*ActorKey.SelectedKeyName.ToString());
}
