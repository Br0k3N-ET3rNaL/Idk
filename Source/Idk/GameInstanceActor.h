// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include <Templates/Function.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GameInstanceActor.generated.h"

class UIdkGameInstance;

/** 
 * Actor containing a reference to the current game instance for convenience.
 * Also used to execute timers when paused.
 */
UCLASS()
class AGameInstanceActor final : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameInstanceActor();

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	/**
	 * Set a timer that runs while the game is paused.
	 * @note Only one timer can be executing at once.
	 * 
	 * @param Callback	Function to call when to timer is finished. 
	 * @param Duration	Duration of the timer to set. 
	 */
	void SetPausedTimer(TFunction<void(void)>&& Callback, float Duration);

private:
	/** Current game instance. @see UIdkGameInstance */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UIdkGameInstance> GameInstance;

	/** Time remaining before executing the callback function. */
	UPROPERTY(VisibleInstanceOnly)
	float TimerDurationRemaining = 0.f;

	/** Function to call when to timer is finished. */
	TFunction<void(void)> TimerCallback;

};
