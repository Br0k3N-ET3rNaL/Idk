// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BTService.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTService_GetMoveToPos.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard service that gets a position for the character to move to. */
UCLASS()
class UBTService_GetMoveToPos final : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_GetMoveToPos(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTNode Interface.
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTService Interface.
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTService Interface

private:
	/** Blackboard key containing the distance from the target to move. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector DistanceFromTarget;

	/** Blackboard key containing the target to move in range of. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Target;

	/** Blackboard key containing the position to move to. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Pos;
};
