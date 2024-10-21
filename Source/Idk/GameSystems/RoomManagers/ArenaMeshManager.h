// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "ArenaMeshManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSetDoorsOpenDelegate, bool)

enum class EDataValidationResult : uint8;
class AArenaRoomDoor;
class UChildActorComponent;
class UInstancedStaticMeshComponent;
class UStaticMesh;
class FDataValidationContext;

/** Manages the doors and walls that surround the arenas. */
UCLASS()
class AArenaMeshManager final : public AActor
{
	GENERATED_BODY()
	
public:	
	AArenaMeshManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * Initialize the mesh manager.
	 * 
	 * @param GapAngles	Angles in radians of the gaps in the walls for the connecting halls. 
	 */
	void Init(const TArray<double>& GapAngles);

	/** Initialize any doors. */
	void InitDoors();

	/** Set whether the front doors should be open. */
	void SetFrontDoorsOpen(const bool bOpen);

	/** Set whether the back doors should be open. */
	void SetBackDoorsOpen(const bool bOpen);

	/**
	 * Get the transform of one of 4 possible gaps in the walls.
	 *
	 * @param bFront	Whether to get the transform of one of the gaps in the front or the back.
	 * @param bLeft		Whether to get the transform of the gap on the left or the right.
	 * @return			Transform of the gap.
	 */
	UE_NODISCARD FTransform GetGapTransform(const bool bFront, const bool bLeft) const;

	/**
	 * Initialize static variables based on supplied parameters and the size of the wall mesh.
	 * 
	 * @param BlueprintClass	Blueprint subclass of AArenaMeshManager.
	 * @param Radius			Radius of the arena.
	 * @param MinGaphWidth		Minimum possible width for the gaps in the walls.
	 */
	static void InitStatics(TSubclassOf<AArenaMeshManager> BlueprintClass, const double Radius, const double MinGapWidth);

	/** Get the nearest valid radius based on the radius set in InitStatics() and the size of the wall mesh. */
	UE_NODISCARD static double GetValidRadius();
	
	/**
	 * Get the minimum valid angle for the front right gap based on supplied parameters
	 * and the size of the wall mesh.
	 * 
	 * @param MinHorizontalRoomSpacing	Minimum horizontal distance between two rooms from a top-down perspective.	 
	 * @param HallLength				Length of the paths between rooms.
	 * @return							Minimum valid angle (in radians) for the front right gap based on
	 *									supplied parameters and the size of the wall mesh.
	 */
	UE_NODISCARD static double GetValidAngle(const double MinHorizontalRoomSpacing, const double HallLength);

	/** Get the width of a gap for a door (in cm). */
	UE_NODISCARD static double GetGapWidth();

	/** Get the distance between the outer edge of the arena and the start of a path (in cm). */
	UE_NODISCARD static double GetPathGapEdgeWidth();

	/** Get the extents of the interior of the arena (in cm). */
	UE_NODISCARD static FVector GetArenaExtents();

private:
	/** Static mesh to use for the arena's walls. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UStaticMesh> WallMesh;

	/** How often to update the door's position when opening and closing. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = "s", ClampMin = 0.01))
	double DoorOpenInterval = 0.25;

	/** How long it takes to open or close a door. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = "s", ClampMin = 0.01))
	double DoorOpenLength = 3.0;

	/** Instanced static mesh component used to spawn the wall meshes. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UInstancedStaticMeshComponent> WallsComp;

	/** Doors at the entrances and exits of the arena. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<AArenaRoomDoor*> Doors;

	/**
	 * Delegate called to open or close the arena's front doors.
	 * 
	 * @param bOpen	Whether to open or close the front doors.
	 */
	FSetDoorsOpenDelegate SetFrontDoorsOpenDelegate;

	/**
	 * Delegate called to open or close the arena's back doors.
	 * 
	 * @param bOpen	Whether to open or close the back doors.
	 */
	FSetDoorsOpenDelegate SetBackDoorsOpenDelegate;

	/** Minimum number of subdivisions for each quadrant of the arena. */
	static const int32 MinSubdivPerQuadrant = 3;

	/** Width of the wall mesh (in cm). */
	static double WallWidth;

	/** Half the width of the wall mesh (in cm). */
	static double HalfWallWidth;

	/** Half the height of the wall mesh (in cm). */
	static double HalfWallHeight;

	/** Half the thickness of the wall mesh (in cm). */
	static double HalfWallDepth;

	/** Position of the center of the wall mesh relative to it's pivot point (in cm). */
	static FVector WallCenter;

	/** Number of subdivisions based on the width of the wall mesh. */
	static int32 Subdivisions;

	/** Angle between each wall from the center of the arena (in radians). */
	static double SubdivAngle;

	/** Half of the angle between the two edges of a gap. Will be a multiple of the subdivision angle (in radians). */
	static double HalfGapAngle;

	/** Angle that the front right door is at (in radians). */
	static double FirstDoorAngle;

	/** Radius of the outer edge of the arena (in cm). */
	static double OuterRadius;

	/** Radius of the inner edge of the arena (in cm). */
	static double InnerRadius;

	/** Width of a gap for a door (in cm). */
	static double DoorWidth;

	/** Position to spawn the first wall at (in cm). */
	static FVector StartPoint;

	/** Extents of the interior of the arena (in cm). */
	static FVector Extents;

};
