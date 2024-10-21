// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/OrientToPlayerComponent.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/IdkGameInstance.h"
#include <Components/SceneComponent.h>
#include <Engine/EngineBaseTypes.h>
#include <Math/MathFwd.h>
#include <Templates/Casts.h>

UOrientToPlayerComponent::UOrientToPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOrientToPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = UIdkGameInstance::GetGameInstance(this)->GetPlayer();
}

void UOrientToPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector ComponentForward;

	if (bUsePlayerForward)
	{
		ComponentForward = -1.0 * Player->GetActorForwardVector();
	}
	else
	{
		const FVector2D ToPlayer = FVector2D(Player->GetActorLocation() - GetComponentLocation());

		ComponentForward = FVector(ToPlayer.GetSafeNormal(), 0.0);
	}
	
	SetWorldRotation(ComponentForward.ToOrientationQuat());
}

void UOrientToPlayerComponent::Init(const bool bInUsePlayerForward)
{
	bUsePlayerForward = bInUsePlayerForward;
}

