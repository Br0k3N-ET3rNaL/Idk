// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameInstanceActor.h"

#include "Idk/IdkGameInstance.h"
#include <Templates/Casts.h>
#include <Templates/Function.h>
#include <Templates/UnrealTemplate.h>

AGameInstanceActor::AGameInstanceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PrimaryActorTick.bTickEvenWhenPaused = true;

}

void AGameInstanceActor::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = CastChecked<UIdkGameInstance>(GetGameInstance());
}

void AGameInstanceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimerDurationRemaining -= DeltaTime;

	if (TimerDurationRemaining <= 0.f)
	{
		TimerCallback();

		SetActorTickEnabled(false);
	}
}

void AGameInstanceActor::SetPausedTimer(TFunction<void(void)>&& Callback, float Duration)
{
	TimerDurationRemaining = Duration;
	TimerCallback = MoveTemp(Callback);

	SetActorTickEnabled(true);
}

