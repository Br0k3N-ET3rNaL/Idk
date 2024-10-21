// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/PortalHallMeshManager.h"

#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <Math/MathFwd.h>

APortalHallMeshManager::APortalHallMeshManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APortalHallMeshManager::Destroyed()
{
	if (LeftTorch)
	{
		LeftTorch->Destroy();
	}

	if (RightTorch)
	{
		RightTorch->Destroy();
	}

	Super::Destroyed();
}

void APortalHallMeshManager::Init(const FVector& Extents)
{
	const FVector& Pos = GetActorLocation();

	const FVector RoofPos = GetTraceHitLocation(Pos, GetActorUpVector() * (2.0 * Extents.Z));
	const FVector FloorPos = GetTraceHitLocation(Pos, GetActorUpVector() * (-2.0 * Extents.Z));
	const double CenterZ = (RoofPos.Z + FloorPos.Z) / 2.0;

	const FVector CenterPos = FVector(Pos.X, Pos.Y, CenterZ);

	// Position in the middle of the left wall
	const FVector LeftPos = GetTraceHitLocation(CenterPos, GetActorRightVector() * (-2.0 * Extents.Y));

	// Position in the middle of the right wall
	const FVector RightPos = GetTraceHitLocation(CenterPos, GetActorRightVector() * (2.0 * Extents.Y));

	// Rotation for the left torch
	const FRotator LeftRotation = GetActorForwardVector().ToOrientationRotator();

	// Rotation for the right torch
	const FRotator RightRotation = (-GetActorForwardVector()).ToOrientationRotator();

	LeftTorch = GetWorld()->SpawnActor<AActor>(TorchClass.LoadSynchronous(), LeftPos, LeftRotation);
	RightTorch = GetWorld()->SpawnActor<AActor>(TorchClass.LoadSynchronous(), RightPos, RightRotation);

	LeftTorch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	RightTorch->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

FVector APortalHallMeshManager::GetTraceHitLocation(const FVector& Start, const FVector& TraceVector) const
{
	FHitResult HitResult;

	const FVector End = Start + TraceVector;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic))
	{
		return HitResult.ImpactPoint;
	}

	// Trace should always hit a wall, however if it doesn't, default to the actor's location
	return GetActorLocation();
}
