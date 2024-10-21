// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BTDecorator.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>

#include "BTDecorator_IsInAbilityRange.generated.h"

class FObjectInitializer;
class UBehaviorTreeComponent;

/** Blackboard decorator that checks if the character is in range for an ability. */
UCLASS()
class UBTDecorator_IsInAbilityRange final : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsInAbilityRange(const FObjectInitializer& ObjectInitializer);

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

	/** Blackboard key containing the target for the ability. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Target;
};
