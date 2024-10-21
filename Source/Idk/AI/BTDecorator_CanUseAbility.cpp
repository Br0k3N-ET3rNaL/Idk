// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTDecorator_CanUseAbility.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include <AIController.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTDecorator_CanUseAbility::UBTDecorator_CanUseAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("Can Use Ability");
}

bool UBTDecorator_CanUseAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	const AIdkEnemyCharacter* Owner = CastChecked<AIdkEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	return Owner->AreAbilityBonusConditionsFulfilled(AbilityIndex);
}

FString UBTDecorator_CanUseAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Can ability %i be used"),
		*Super::GetStaticDescription(), 
		AbilityIndex);
}
