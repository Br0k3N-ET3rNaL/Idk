// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/AI/BTDecorator_IsInAbilityRange.h"

#include "Idk/Character/IdkEnemyCharacter.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h> 
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

UBTDecorator_IsInAbilityRange::UBTDecorator_IsInAbilityRange(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = TEXT("Is In Ability Range");

	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsInAbilityRange, Target), AActor::StaticClass());
}

bool UBTDecorator_IsInAbilityRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	check(Blackboard);

	const AActor* TargetActor = CastChecked<AActor>(Blackboard->GetValueAsObject(Target.SelectedKeyName));
	const AIdkEnemyCharacter* Owner = CastChecked<AIdkEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	return Owner->IsInAbilityRange(TargetActor, AbilityIndex);
}

FString UBTDecorator_IsInAbilityRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Check if in range of %s"),
		*Super::GetStaticDescription(),
		*Target.SelectedKeyName.ToString());
}
