// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/HallMeshManager.h"

#include <Components/InstancedStaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include <Engine/StaticMesh.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Math/UnrealPlatformMathSSE.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/SubclassOf.h>
#include <UObject/UObjectGlobals.h>

double AHallMeshManager::WallWidth = 0.0;
double AHallMeshManager::HalfWallWidth = 0.0;
double AHallMeshManager::HalfWallHeight = 0.0;
double AHallMeshManager::HalfWallDepth = 0.0;
double AHallMeshManager::HalfHallWidth = 0.0;
FVector AHallMeshManager::WallCenter;
int32 AHallMeshManager::Subdivisions = 0;
FVector AHallMeshManager::StartCenter;
FVector AHallMeshManager::Extents;

AHallMeshManager::AHallMeshManager()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	WallsComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallsComp"));

	WallsComp->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
EDataValidationResult AHallMeshManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (WallMesh.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Wall mesh is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AHallMeshManager::Init()
{
	check(!WallMesh.IsNull());

	WallsComp->SetStaticMesh(WallMesh.LoadSynchronous());

	WallsComp->SetRelativeLocation(FVector(0.0, 0.0, HalfWallHeight));

	const FVector Offset = FVector(0.0, HalfHallWidth, 0.0);
	FVector Center = StartCenter;

	for (int32 i = 0; i < Subdivisions; ++i)
	{
		WallsComp->AddInstance(FTransform(Center - Offset));
		WallsComp->AddInstance(FTransform(Center + Offset));

		Center.X += WallWidth;
	}
}

FTransform AHallMeshManager::GetDoorTransform(const bool bFront) const
{
	// X offset from the start point to one of the ends
	const double HallEndOffsetX = (bFront) ? Subdivisions * WallWidth : 0.0;

	// X offset from the pivot of a wall to it's end
	const double WallEndOffsetX = -(HalfWallWidth - WallCenter.X);

	FVector RelativePos = StartCenter + FVector(HallEndOffsetX + WallEndOffsetX, 0.0, 0.0);

	// Apply rotation
	RelativePos = RelativePos.RotateAngleAxis(GetActorRotation().Yaw, GetActorUpVector());

	return FTransform(GetActorQuat(), GetActorLocation() + RelativePos);
}

void AHallMeshManager::InitStatics(TSubclassOf<AHallMeshManager> BlueprintClass, const double MinLength, const double InnerWidth)
{
	AHallMeshManager* Default = BlueprintClass.GetDefaultObject();

	check(!Default->WallMesh.IsNull());
	UStaticMesh* Wall = Default->WallMesh.LoadSynchronous();

	const FVector WallSize = Wall->GetBoundingBox().GetSize();

	WallWidth = WallSize.X;
	HalfWallWidth = WallWidth / 2.0;
	HalfWallHeight = WallSize.Z / 2.0;
	HalfWallDepth = WallSize.Y / 2.0;

	HalfHallWidth = (InnerWidth / 2.0);

	WallCenter = Wall->GetBoundingBox().GetCenter();

	const double UnroundedSubdiv = MinLength / WallWidth;
	Subdivisions = FMath::CeilToInt32(UnroundedSubdiv);

	// X offset from the pivot of a wall to it's end
	const double WallEndOffsetX = HalfWallWidth - WallCenter.X;

	// X offset from the center of the room to the back end
	const double StartOffsetX = -((Subdivisions / 2.0) * WallWidth);

	StartCenter = FVector(StartOffsetX + WallEndOffsetX, 0.0, 0.0);

	Extents = FVector(Subdivisions * HalfWallWidth, HalfHallWidth, HalfWallHeight);
}

double AHallMeshManager::GetCalculatedLength()
{
	return Subdivisions * WallWidth;
}

double AHallMeshManager::GetCalculatedWidth()
{
	return (HalfHallWidth + HalfWallDepth) * 2.0;
}

FVector AHallMeshManager::GetHallExtents()
{
	return Extents;
}
