// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/SceneComponent.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "OrientToPlayerComponent.generated.h"

enum ELevelTick;
class AIdkPlayerCharacter;
struct FActorComponentTickFunction;

/** Component that always faces the player. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UOrientToPlayerComponent final : public USceneComponent
{
	GENERATED_BODY()

public:	
	UOrientToPlayerComponent();

	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	/**
	 * Initialize the component.
	 * 
	 * @param bUsePlayerForward	Whether to mirror the direction the player is facing or face the player directly. 
	 */
	void Init(const bool bUsePlayerForward);

private:
	/** Whether to mirror the direction the player is facing or face the player directly. */
	bool bUsePlayerForward = false;

	/** Current player character. */
	TObjectPtr<AIdkPlayerCharacter> Player;

};
