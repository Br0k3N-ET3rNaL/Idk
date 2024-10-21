// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTService_GetAbilityMoveToPos.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTService_GetAbilityMoveToPos::UBTService_GetAbilityMoveToPos(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCallTickOnSearchStart = true;
	bRestartTimerOnEachActivation = true;

	NodeName = TEXT("Get Ability MoveTo Pos");

	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetAbilityMoveToPos, Target), AActor::StaticClass());

	Pos.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetAbilityMoveToPos, Pos));
}

void UBTService_GetAbilityMoveToPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	if (const AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(Target.SelectedKeyName)))
	{
		const AIdkEnemyCharacter* Owner = CastChecked<AIdkEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

		const FVector MoveToPos = Owner->GetPosInAbilityRange(TargetActor, AbilityIndex);

		Blackboard->SetValueAsVector(Pos.SelectedKeyName, MoveToPos);
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

FString UBTService_GetAbilityMoveToPos::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Pos in range of %s"),
		*Super::GetStaticDescription(),
		*Target.SelectedKeyName.ToString());
}
