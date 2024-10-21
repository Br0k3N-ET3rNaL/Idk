// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BTDecorator.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTDecorator_IsActorValid.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard decorator that checks if an actor is valid. */
UCLASS()
class UBTDecorator_IsActorValid final : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsActorValid(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTNode Interface.
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTDecorator Interface.
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	//~ End UBTDecorator Interface

private:
	/** Blackboard key containing the actor to check. */
	UPROPERTY(EditAnywhere, Category = Blackboard, meta = (DisplayName = "Actor"))
	FBlackboardKeySelector ActorKey;
};
