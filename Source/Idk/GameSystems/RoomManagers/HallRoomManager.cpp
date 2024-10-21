// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"

#include "Idk/GameSystems/Portal.h"
#include "Idk/GameSystems/RoomManagers/HallMeshManager.h"
#include "Idk/GameSystems/RoomManagers/PortalHallMeshManager.h"
#include "Idk/GameSystems/RoomManagers/RoomManager.h"
#include "Idk/GameSystems/RoomSpawnInfo.h"
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult AHallRoomManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (MeshManagerClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Mesh manager class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (PortalMeshManagerClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Portal mesh manager class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AHallRoomManager::Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents, const bool bPortal)
{
	Super::Init(RoomSpawnInfo, BoxExtents);

	bSpawnPortal = bPortal;

	SetActorRelativeRotation(FQuat(GetActorUpVector(), RoomSpawnInfo.GetAngle()));

	MeshManager = GetWorld()->SpawnActor<AHallMeshManager>(MeshManagerClass.LoadSynchronous());
	MeshManager->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	MeshManager->Init();
}

FTransform AHallRoomManager::GetPortalSource() const
{
	return MeshManager->GetDoorTransform(true);
}

FTransform AHallRoomManager::GetPortalDestination() const
{
	return MeshManager->GetDoorTransform(false);
}

void AHallRoomManager::Destroyed()
{
	if (MeshManager)
	{
		MeshManager->Destroy();
	}

	if (PortalMeshManager)
	{
		PortalMeshManager->Destroy();
	}

	Super::Destroyed();
}

void AHallRoomManager::OnEnabled()
{
	Super::OnEnabled();

	if (bSpawnPortal)
	{
		PortalMeshManager = GetWorld()->SpawnActor<APortalHallMeshManager>(PortalMeshManagerClass.LoadSynchronous());
		PortalMeshManager->GetRootComponent()->AttachToComponent(
			RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		PortalMeshManager->SetActorLocation(BoxCollisionComp->GetComponentLocation());
		PortalMeshManager->Init(BoxCollisionComp->GetUnscaledBoxExtent());
	}
}

void AHallRoomManager::OnDisabled()
{
	Super::OnDisabled();

	if (PortalMeshManager)
	{
		PortalMeshManager->Destroy();
	}

	if (ExitCollision)
	{
		ExitCollision->DestroyComponent();
	}
}

void AHallRoomManager::OnRoomActivated()
{
	if (!bHandleCompletion)
	{
		ExitCollision = NewObject<UBoxComponent>(this);
		ExitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ExitCollision->RegisterComponent();
		ExitCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

		ExitCollision->SetBoxExtent(FVector(200.0, BoxCollisionComp->GetScaledBoxExtent().Y, BoxCollisionComp->GetScaledBoxExtent().Z));
		ExitCollision->SetWorldLocation(MeshManager->GetDoorTransform(true).GetLocation());

		ExitCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ExitCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		ExitCollision->OnComponentBeginOverlap.AddDynamic(this, &AHallRoomManager::OnExitBeginOverlap);

		ExitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (bSpawnPortal)
	{
		SpawnPortal();

		Portal->OnPortalEnteredDelegate.BindUObject(this, &AHallRoomManager::Complete);
	}

	Super::OnRoomActivated();
}

void AHallRoomManager::OnCompletion()
{
	Super::OnCompletion();

	if (ExitCollision)
	{
		ExitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (bSpawnPortal && !Portal)
	{
		SpawnPortal();
	}
}

void AHallRoomManager::SpawnPortal()
{
	check(bSpawnPortal);
	check(GetPortalDestDelegate.IsBound());

	const FTransform Source = GetPortalSource();
	const FTransform Destination = GetPortalDestDelegate.Execute();

	Super::SpawnPortal(Source, Destination);
}

void AHallRoomManager::OnExitBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AIdkPlayerCharacter>())
	{
		ExitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Complete();
	}
}
