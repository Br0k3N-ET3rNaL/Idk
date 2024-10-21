// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BTDecorator.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTDecorator_CanUseAbility.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard decorator that checks if the character can use a specific ability. */
UCLASS()
class UBTDecorator_CanUseAbility final : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanUseAbility(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTNode Interface.
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTDecorator Interface.
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	//~ End UBTDecorator Interface

private:
	/** Index of the ability to check. */
	UPROPERTY(EditAnywhere)
	int32 AbilityIndex = -1;
	
};
