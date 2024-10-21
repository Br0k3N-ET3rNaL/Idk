// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BTTaskNode.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTTask_UseAbility.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard task for using a character's ability. */
UCLASS()
class UBTTask_UseAbility final : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseAbility(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTNode Interface.
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTTaskNode Interface.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTTaskNode Interface

private:
	/** Index of the ability to use. */
	UPROPERTY(EditAnywhere)
	int32 AbilityIndex = -1;

	/** Blackboard key containing whether the character is currently using the ability. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector IsUsingAbility;

	/** Blackboard key containing the target for the ability. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Target;
};
