// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <AIController.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "IdkAIController.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class AIdkPlayerCharacter;
class APawn;
class UBehaviorTree;

/** Controller class used by AI characters. */
UCLASS(Abstract, Blueprintable)
class AIdkAIController final : public AAIController
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

private:
	//~ Begin AController Interface.
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AController Interface

	/** Set the current player character. */
	void SetPlayer(AIdkPlayerCharacter& Player);

	/** Behavior tree to use for the AI. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings", meta = (BlueprintBaseOnly))
	TSoftObjectPtr<UBehaviorTree> BehaviorTree;

	/** Name of the blackboard key that represents the current player character. */
	static const FName PlayerKey;

};
