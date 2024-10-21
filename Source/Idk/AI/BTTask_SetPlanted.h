// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BTTaskNode.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTTask_SetPlanted.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard task for to set the planted state of a siege enemy. @see ASiegeEnemy */
UCLASS()
class UBTTask_SetPlanted final : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetPlanted(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTNode Interface.
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTTaskNode Interface.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTTaskNode Interface

private:
	/**
	 * Event called when the siege enemy has finished planting.
	 * 
	 * @param OwnerComp	Blackboard component of the siege enemy. 
	 */
	void OnFinishedPlanting(UBehaviorTreeComponent* OwnerComp);

	/** Whether to have the siege enemy enter or exit the planted state. */
	UPROPERTY(EditAnywhere)
	bool bPlanted = false;

	/** Blackboard key containing whether the siege enemy is currently planted. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector IsPlanted;
};
