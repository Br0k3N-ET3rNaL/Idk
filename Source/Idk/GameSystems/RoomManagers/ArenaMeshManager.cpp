// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/ArenaMeshManager.h"

#include "Idk/GameSystems/RoomManagers/ArenaRoomDoor.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <GenericPlatform/GenericPlatformMath.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Math/UnrealPlatformMathSSE.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/SubclassOf.h>
#include <UObject/UObjectGlobals.h>

double AArenaMeshManager::WallWidth = 0.0;
double AArenaMeshManager::HalfWallWidth = 0.0;
double AArenaMeshManager::HalfWallHeight = 0.0;
double AArenaMeshManager::HalfWallDepth = 0.0;
FVector AArenaMeshManager::WallCenter = FVector::ZeroVector;
int32 AArenaMeshManager::Subdivisions = 0;
double AArenaMeshManager::SubdivAngle = 0.0;
double AArenaMeshManager::HalfGapAngle = 0.0;
double AArenaMeshManager::FirstDoorAngle = 0.0;
double AArenaMeshManager::OuterRadius = 0.0;
double AArenaMeshManager::InnerRadius = 0.0;
double AArenaMeshManager::DoorWidth = 0.0;
FVector AArenaMeshManager::StartPoint = FVector::ZeroVector;
FVector AArenaMeshManager::Extents = FVector::ZeroVector;

AArenaMeshManager::AArenaMeshManager()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	WallsComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallsComp"));

	WallsComp->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
EDataValidationResult AArenaMeshManager::IsDataValid(FDataValidationContext& Context) const
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

void AArenaMeshManager::Destroyed()
{
	for (AArenaRoomDoor* Door : Doors)
	{
		Door->Destroy();
	}

	Super::Destroyed();
}

void AArenaMeshManager::Init(const TArray<double>& GapAngles)
{
	check(!WallMesh.IsNull());
	UStaticMesh& Wall = *WallMesh.LoadSynchronous();

	WallsComp->SetStaticMesh(&Wall);

	const double WallZOffset = WallCenter.Z + HalfWallHeight;
	WallsComp->SetRelativeLocation(FVector(0.0, 0.0, WallZOffset));

	UWorld* World = GetWorld();

	for (int32 i = 0; i < GapAngles.Num(); ++i)
	{
		AArenaRoomDoor* Door = World->SpawnActor<AArenaRoomDoor>(AArenaRoomDoor::StaticClass());
		Door->GetRootComponent()->AttachToComponent(WallsComp, FAttachmentTransformRules::SnapToTargetIncludingScale);

		Door->SetDoorMesh(Wall);

		const double DoorAngle = GapAngles[i];

		if (DoorAngle > UE_DOUBLE_HALF_PI && DoorAngle < 3.0 * UE_DOUBLE_HALF_PI)
		{
			// Back doors
			SetBackDoorsOpenDelegate.AddUObject(Door, &AArenaRoomDoor::SetDoorOpen);
		}
		else
		{
			// Front doors
			SetFrontDoorsOpenDelegate.AddUObject(Door, &AArenaRoomDoor::SetDoorOpen);
		}

		Doors.Add(Door);
	}

	FVector WallPos = StartPoint;

	const double AngleThreshold = SubdivAngle / 4.0;

	for (int32 i = 0; i < Subdivisions; ++i)
	{
		const double WallAngle = (i + 0.5) * SubdivAngle;

		const int32 GapIndex = GapAngles.IndexOfByPredicate([=](const double Angle) {
			return FMath::Abs(WallAngle - Angle) < HalfGapAngle;
			});

		const FTransform WallTransform = FTransform(FQuat(GetActorUpVector(), WallAngle + UE_DOUBLE_HALF_PI), WallPos);

		if (GapIndex != INDEX_NONE)
		{
			Doors[GapIndex]->AddDoorInstance(WallTransform);
		}
		else
		{
			WallsComp->AddInstance(WallTransform);
		}
		
		WallPos = WallPos.RotateAngleAxisRad(SubdivAngle, GetActorUpVector());
	}
}

void AArenaMeshManager::InitDoors()
{
	for (AArenaRoomDoor* Door : Doors)
	{
		Door->Init();
	}
}

void AArenaMeshManager::SetFrontDoorsOpen(const bool bOpen)
{
	SetFrontDoorsOpenDelegate.Broadcast(bOpen);
}

void AArenaMeshManager::SetBackDoorsOpen(const bool bOpen)
{
	SetBackDoorsOpenDelegate.Broadcast(bOpen);
}

FTransform AArenaMeshManager::GetGapTransform(const bool bFront, const bool bLeft) const
{
	double GapAngle;
	double ForwardAngle;

	if (bFront && !bLeft)
	{
		GapAngle = FirstDoorAngle;
		ForwardAngle = GapAngle;
	}
	else if (!bFront && !bLeft)
	{
		GapAngle = UE_DOUBLE_PI - FirstDoorAngle;
		ForwardAngle = UE_DOUBLE_TWO_PI - FirstDoorAngle;
	}
	else if (!bFront && bLeft)
	{
		GapAngle = UE_DOUBLE_PI + FirstDoorAngle;
		ForwardAngle = FirstDoorAngle;
	}
	else /*if (bFront && bLeft)*/
	{
		GapAngle = UE_DOUBLE_TWO_PI - FirstDoorAngle;
		ForwardAngle = GapAngle;
	}

	const FQuat GapRotation = FQuat(GetActorUpVector(), GapAngle);
	const FQuat ForwardRotation = FQuat(GetActorUpVector(), ForwardAngle);
	const FVector FrontCenterPos = FVector(InnerRadius - HalfWallDepth, 0.0, 0.0);
	const FVector RelativePos = GapRotation.RotateVector(FrontCenterPos);
	const FVector Pos = GetActorLocation() + RelativePos;

	return FTransform(ForwardRotation, Pos);
}

void AArenaMeshManager::InitStatics(TSubclassOf<AArenaMeshManager> BlueprintClass, const double Radius, const double MinGapWidth)
{
	AArenaMeshManager* Default = BlueprintClass.GetDefaultObject();

	check(!Default->WallMesh.IsNull());

	UStaticMesh* Wall = Default->WallMesh.LoadSynchronous();

	const FBox WallBounds = Wall->GetBoundingBox();
	const FVector WallSize = WallBounds.GetSize();
	WallCenter = WallBounds.GetCenter();

	WallWidth = WallSize.X;
	HalfWallWidth = WallWidth / 2.0;
	HalfWallHeight = WallSize.Z / 2.0;
	HalfWallDepth = WallSize.Y / 2.0;

	// Total angle of each quadrant in radians
	const double QuadrantAngle = UE_DOUBLE_HALF_PI;

	const double BaseSubdivAngle = (2.0 * FMath::Asin(HalfWallWidth / Radius));
	const double SubdivPerQuadrantUnrounded = QuadrantAngle / BaseSubdivAngle;

	// Number of subdivisions for each quadrant of the arena
	int32 SubdivPerQuadrant = FMath::Max(MinSubdivPerQuadrant, FMath::RoundToInt32(SubdivPerQuadrantUnrounded));

	SubdivAngle = QuadrantAngle / SubdivPerQuadrant;

	OuterRadius = HalfWallWidth / FMath::Sin(SubdivAngle / 2.0);

	// Number of subdivisions for each door
	uint8 SubdivPerDoor = 1;
	DoorWidth = WallWidth;
	HalfGapAngle = SubdivAngle / 2.0;

	// Increase the subdivisions per door until the width of the door is greater than or equal to the minimum width
	while (MinGapWidth > DoorWidth && !FMath::IsNearlyEqual(MinGapWidth, DoorWidth, 1.0))
	{
		++SubdivPerDoor;
		++SubdivPerQuadrant;

		SubdivAngle = QuadrantAngle / SubdivPerQuadrant;
		OuterRadius = HalfWallWidth / FMath::Sin(SubdivAngle / 2.0);

		HalfGapAngle = (SubdivPerDoor / 2.0) * SubdivAngle;
		DoorWidth = 2.0 * (FMath::Sin(HalfGapAngle) * OuterRadius);
	}

	Subdivisions = 4 * SubdivPerQuadrant;

	InnerRadius = FMath::Cos(SubdivAngle / 2.0) * OuterRadius;

	// Position of the pivot point of the first wall to spawn
	const FVector FirstWallPivotPos = FVector(InnerRadius, -WallCenter.X, 0.0);

	// Rotate the point so that the left edge of the first wall is at the center
	StartPoint = FirstWallPivotPos.RotateAngleAxisRad(SubdivAngle / 2.0, FVector::UpVector);

	const double InnerBoxExtent = (UE_DOUBLE_SQRT_2 / 2.0) * OuterRadius;

	Extents = FVector(InnerBoxExtent, InnerBoxExtent, HalfWallHeight);

	AArenaRoomDoor::InitStatics(2.0 * HalfWallHeight, Default->DoorOpenInterval, Default->DoorOpenLength);
}

double AArenaMeshManager::GetValidRadius()
{
	return OuterRadius + WallCenter.Y + HalfWallDepth;
}

double AArenaMeshManager::GetValidAngle(const double MinHorizontalRoomSpacing, const double HallLength)
{
	const double Adjacent = OuterRadius + (MinHorizontalRoomSpacing / 2.0);
	const double Hypotenuse = (2.0 * OuterRadius) + HallLength;
	const double RoomSpacingRatio = Adjacent / Hypotenuse;

	const double MinValidAngle = FMath::Asin(RoomSpacingRatio);
	const double StartAngle = SubdivAngle + HalfGapAngle;
	double ValidAngle = StartAngle;

	while (ValidAngle < MinValidAngle)
	{
		ValidAngle += SubdivAngle;
	}

	const double MaxValidAngle = FMath::Acos(RoomSpacingRatio);
	const double LastValidAngle = UE_DOUBLE_HALF_PI - StartAngle;

	if (ValidAngle < MaxValidAngle && ValidAngle <= LastValidAngle)
	{
		FirstDoorAngle = ValidAngle;

		return ValidAngle;
	}

	return 0.0;
}

double AArenaMeshManager::GetGapWidth()
{
	return DoorWidth;
}

double AArenaMeshManager::GetPathGapEdgeWidth()
{
	const double DistToGap = FMath::Cos(HalfGapAngle) * OuterRadius;

	return OuterRadius - DistToGap;
}

FVector AArenaMeshManager::GetArenaExtents()
{
	return Extents;
}
