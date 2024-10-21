// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "PortalHallMeshManager.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;

/** Manages the torches used to light up underground paths connected by portals. */
UCLASS()
class APortalHallMeshManager final : public AActor
{
	GENERATED_BODY()
	
public:
	APortalHallMeshManager();

	//~ Begin AActor Interface.
	virtual void Destroyed() override final;
	//~ End AActor Interface

	/**
	 * Initialize the mesh manager.
	 * 
	 * @param Extents	Inner extents of the room.
	 */
	void Init(const FVector& Extents);

private:
	/**
	 * Get the position of a hit when performing a line trace.
	 * 
	 * @param Start			Location to start trace from.
	 * @param TraceVector	Vector representing the line trace.
	 * @return				Location of the line trace hit.
	 */
	UE_NODISCARD FVector GetTraceHitLocation(const FVector& Start, const FVector& TraceVector) const;

	/** Blueprint class representing a torch. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<AActor> TorchClass;

	/** Torch on the left side of the hall. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> LeftTorch;

	/** Torch on the right side of the hall. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> RightTorch;

};
