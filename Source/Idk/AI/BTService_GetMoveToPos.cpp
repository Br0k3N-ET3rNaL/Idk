// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTService_GetMoveToPos.h"

#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTService_GetMoveToPos::UBTService_GetMoveToPos(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCallTickOnSearchStart = true;
	bRestartTimerOnEachActivation = true;

	NodeName = TEXT("Get MoveTo Pos");

	DistanceFromTarget.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetMoveToPos, DistanceFromTarget));

	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetMoveToPos, Target), AActor::StaticClass());

	Pos.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetMoveToPos, Pos));
}

void UBTService_GetMoveToPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	const FVector CurrentPos = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();

	const AActor* TargetActor = CastChecked<AActor>(Blackboard->GetValueAsObject(Target.SelectedKeyName));
	const FVector TargetPos = TargetActor->GetActorLocation();

	FVector MoveVector = CurrentPos - TargetPos;
	MoveVector.Normalize();

	const float Distance = Blackboard->GetValueAsFloat(DistanceFromTarget.SelectedKeyName);

	MoveVector *= Distance;

	Blackboard->SetValueAsVector(Pos.SelectedKeyName, TargetPos + MoveVector);

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

FString UBTService_GetMoveToPos::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s cm from %s"),
		*Super::GetStaticDescription(),
		*DistanceFromTarget.SelectedKeyName.ToString(),
		*Target.SelectedKeyName.ToString());
}
