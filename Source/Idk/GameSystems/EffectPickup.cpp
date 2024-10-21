// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/EffectPickup.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include <Components/CapsuleComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SceneComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <NiagaraComponent.h>
#include <Templates/Casts.h>

AEffectPickup::AEffectPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	CapsuleCollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CaspuleCollisionComp"));
	PickupVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupVFX"));

	// Initialize the capsule collision
	CapsuleCollisionComp->SetupAttachment(RootComponent);
	CapsuleCollisionComp->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	CapsuleCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEffectPickup::OnCapsuleBeginOverlap);

	PickupVFX->SetupAttachment(RootComponent);
}

void AEffectPickup::BeginPlay()
{
	Super::BeginPlay();

	const double CapsuleHalfHeight = CapsuleCollisionComp->GetScaledCapsuleHalfHeight();

	CapsuleCollisionComp->SetRelativeLocation(FVector(0.0, 0.0, CapsuleHalfHeight));
}

void AEffectPickup::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AIdkPlayerCharacter* Player = CastChecked<AIdkPlayerCharacter>(OtherActor))
	{
		OnPickup(*Player);

		OnPickedUpDelegate.ExecuteIfBound();

		Destroy();
	}
}
