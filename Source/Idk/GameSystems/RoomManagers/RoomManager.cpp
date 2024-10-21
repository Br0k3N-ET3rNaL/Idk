// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/RoomManagers/RoomManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/Portal.h"
#include "Idk/GameSystems/RoomSpawnInfo.h"
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SceneComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

ARoomManager::ARoomManager()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
}

#if WITH_EDITOR
EDataValidationResult ARoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PortalClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARoomManager: Portal class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ARoomManager::Destroyed()
{
	if (Portal)
	{
		Portal->Destroy();
	}

	Super::Destroyed();
}

void ARoomManager::Enable()
{
	check(!bEnabled);

	bEnabled = true;

	// Spawn the room's hitbox
	BoxCollisionComp = NewObject<UBoxComponent>(this, TEXT("BoxCollisionComp"));
	BoxCollisionComp->RegisterComponent();
	BoxCollisionComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	// Setup collisions
	BoxCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARoomManager::OnBoxBeginOverlap);
	BoxCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->SetBoxExtent(BoxCollisionExtents);
	BoxCollisionComp->AddRelativeLocation(FVector(0.0, 0.0, BoxCollisionExtents.Z));

	OnEnabled();
}

void ARoomManager::Disable()
{
	if (BoxCollisionComp)
	{
		BoxCollisionComp->DestroyComponent();
	}

	if (Portal)
	{
		Portal->Destroy();
	}

	Rng = nullptr;

	if (bEnabled)
	{
		OnDisabled();
	}
}

void ARoomManager::Activate()
{
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Initialize the random number generator
	Rng = &UIdkRandomStream::CreateRandomStream(this, Seed);

	OnRoomActivated();

	RoomActivatedDelegate.Broadcast();
}

void ARoomManager::Complete()
{
	if (!bComplete)
	{
		bComplete = true;

		BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OnCompletion();

		RoomCompletedDelegate.Broadcast();
	}
}

void ARoomManager::OnEnabled()
{
}

void ARoomManager::OnDisabled()
{
}

void ARoomManager::OnRoomActivated()
{
}

void ARoomManager::OnCompletion()
{
}

void ARoomManager::Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents)
{
	Level = RoomSpawnInfo.GetLevel();
	Seed = RoomSpawnInfo.GetSeed();

#if WITH_EDITORONLY_DATA
	SeedString = UIdkRandomStream::SeedToString(Seed);
#endif

	BoxCollisionExtents = BoxExtents;
}

void ARoomManager::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AIdkPlayerCharacter>())
	{
		Player = CastChecked<AIdkPlayerCharacter>(OtherActor);

		Activate();
	}
}

void ARoomManager::SpawnPortal(const FTransform& Src, const FTransform& Dest)
{
	const FTransform SpawnTransform = RootComponent->GetComponentTransform();

	Portal = GetWorld()->SpawnActorDeferred<APortal>(
		PortalClass.LoadSynchronous(), SpawnTransform, this);

	Portal->Init(Src, Dest);

	UGameplayStatics::FinishSpawningActor(Portal, SpawnTransform);

	Portal->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}
