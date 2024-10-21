// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "HallMeshManager.generated.h"

enum class EDataValidationResult : uint8;
class UStaticMesh;
class UInstancedStaticMeshComponent;
class FDataValidationContext;

/** Manages the walls for the halls between arenas. */
UCLASS()
class AHallMeshManager final : public AActor
{
	GENERATED_BODY()
	
public:	
	AHallMeshManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Initialize the hall mesh manager. */
	void Init();

	/**
	 * Get the transform of one of the hall's doors.
	 * 
	 * @param bFront	Whether to get the transform of the front or back door. 
	 * @return			The transform of the door. 
	 */
	UE_NODISCARD FTransform GetDoorTransform(const bool bFront) const;

	/**
	 * Initialize static variables based on supplied parameters and the size of the wall mesh.
	 * 
	 * @param BlueprintClass	Blueprint subclass of AHallMeshManager.
	 * @param MinLength			Minimum length of the hall 
	 * @param InnerWidth		Width of the inner edge of the hall. 
	 */
	static void InitStatics(TSubclassOf<AHallMeshManager> BlueprintClass, const double MinLength, const double InnerWidth);

	/** Get the calculated length of a hall (in cm). */
	UE_NODISCARD static double GetCalculatedLength();

	/** Get the calculated width of a hall (in cm). */
	UE_NODISCARD static double GetCalculatedWidth();

	/** Get the extents of the interior of the hall (in cm). */
	UE_NODISCARD static FVector GetHallExtents();

private:
	/** Static mesh to use for the hall's walls. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UStaticMesh> WallMesh;

	/** Instanced static mesh component used to spawn the wall meshes. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UInstancedStaticMeshComponent> WallsComp;

	/** Width of the wall mesh (in cm). */
	static double WallWidth;

	/** Half the width of the wall mesh (in cm). */
	static double HalfWallWidth;

	/** Half the height of the wall mesh (in cm). */
	static double HalfWallHeight;

	/** Half the thickness of the wall mesh (in cm). */
	static double HalfWallDepth;

	/** Half the width of a hall (in cm). */
	static double HalfHallWidth;

	/** Position of the center of the wall mesh relative to it's pivot point (in cm). */
	static FVector WallCenter;

	/** Number of subdivisions based on the width of the wall mesh. */
	static int32 Subdivisions;

	/** Relative position of the center of the starting end of the hall (in cm). */
	static FVector StartCenter;

	/** Extents of the interior of the hall (in cm). */
	static FVector Extents;

};
