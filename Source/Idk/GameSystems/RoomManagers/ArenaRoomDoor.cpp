// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomManagers/ArenaRoomDoor.h"

#include "Idk/IdkGameInstance.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include <Components/TimelineComponent.h>
#include <Curves/CurveFloat.h>
#include <Engine/StaticMesh.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/PimplPtr.h>

double AArenaRoomDoor::DoorHeight = 0.0;
double AArenaRoomDoor::TimelineInterval = 0.0;
double AArenaRoomDoor::TimelineLength = 0.0;

AArenaRoomDoor::AArenaRoomDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	DoorMeshComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("DoorMeshComp"));

	DoorMeshComp->SetupAttachment(RootComponent);
}

void AArenaRoomDoor::Init()
{
	// Initialize the timeline
	Timeline = MakePimpl<FTimeline>();
	Timeline->SetTimelineLength(TimelineLength);
	Timeline->AddInterpFloat(&UIdkGameInstance::GetGameInstance(this)->GetDoorOpenCurve(), FOnTimelineFloatStatic::CreateUObject(this, &AArenaRoomDoor::SetDoorOpenPercent));
}

void AArenaRoomDoor::SetDoorMesh(UStaticMesh& DoorMesh)
{
	DoorMeshComp->SetStaticMesh(&DoorMesh);
}

void AArenaRoomDoor::SetDoorOpen(const bool bInOpen)
{
	if (bInOpen != bOpen)
	{
		if (bInOpen)
		{
			Timeline->Play();
		}
		else
		{
			// Play the timeline in reverse
			Timeline->Reverse();
		}

		bOpen = !bOpen;

		// Start evaluating timeline
		GetWorldTimerManager().SetTimer(Timer, this, &AArenaRoomDoor::TickTimeline, TimelineInterval, true, 0.f);
	}
}

int32 AArenaRoomDoor::AddDoorInstance(const FTransform& InstanceTransform)
{
	return DoorMeshComp->AddInstance(InstanceTransform, false);
}

void AArenaRoomDoor::InitStatics(const double InDoorHeight, const double InTimelineInterval, const double InTimelineLength)
{
	DoorHeight = InDoorHeight;
	TimelineInterval = InTimelineInterval;
	TimelineLength = InTimelineLength;
}

void AArenaRoomDoor::SetDoorOpenPercent(const float OpenPercent)
{
	DoorMeshComp->SetRelativeLocation(FVector(0.0, 0.0, -(OpenPercent * DoorHeight)));
}

void AArenaRoomDoor::TickTimeline()
{
	if (Timeline->IsPlaying())
	{
		// Evaluate the timeline
		Timeline->TickTimeline(TimelineInterval);
	}
	else
	{
		// Stop evaluating the timeline
		GetWorldTimerManager().ClearTimer(Timer);
	}
}
