// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/TimerHandle.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/PimplPtr.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "ArenaRoomDoor.generated.h"

class UCurveFloat;
class UInstancedStaticMeshComponent;
class UStaticMesh;
struct FTimeline;

/** Doors used between arenas and halls. */
UCLASS()
class AArenaRoomDoor final : public AActor
{
	GENERATED_BODY()
	
public:	
	AArenaRoomDoor();

	/** Initialize the door so that it can be opened and closed. */
	void Init();

	/** Set the static mesh to use for the door. */
	void SetDoorMesh(UStaticMesh& DoorMesh);

	/** Open or close the door. */
	void SetDoorOpen(const bool bInOpen);

	/**
	 * Spawn another instance of the door mesh.
	 * 
	 * @param InstanceTransform Transform in local space used to spawn the instance.
	 * @return					Index of the instance.
	 */
	int32 AddDoorInstance(const FTransform& InstanceTransform);

	/**
	 * Initialize static variables based on supplied parameters.
	 * 
	 * @param InDoorHeight			Height of the door mesh.
	 * @param InTimelineInterval	How often to update the door position when interpolating.
	 * @param InTimelineLength		How long to interp between open and closed.
	 */
	static void InitStatics(const double InDoorHeight, const double InTimelineInterval, const double InTimelineLength);

private:
	/**
	 * Set how open the door should be.
	 * 
	 * @param OpenPercent	Between 0 and 1, where 0 is closed, and 1 is open.
	 */
	void SetDoorOpenPercent(const float OpenPercent);

	/** Evaluate the timeline. */
	void TickTimeline();

	/** Component used for the door mesh. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> DoorMeshComp;

	/** Timeline that is evaluated to interp between open and closed. */
	TPimplPtr<FTimeline> Timeline;

	/** Timer used to evaluate the timeline to interp between open and closed. */
	FTimerHandle Timer;

	/** Whether the door is currently open. */
	bool bOpen = false;

	/** Height of the door mesh. */
	static double DoorHeight;

	/** How often to update the door position when interpolating. */
	static double TimelineInterval;

	/** How long to interp between open and closed. */
	static double TimelineLength;

};
