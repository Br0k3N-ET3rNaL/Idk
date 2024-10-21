// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/RoomManager.h"
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "HallRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AActor;
class AHallMeshManager;
class APortal;
class APortalHallMeshManager;
class FDataValidationContext;
class UBoxComponent;
class UPrimitiveComponent;
struct FHitResult;
struct FPortalSpawnInfo;
struct FRoomSpawnInfo;

DECLARE_DELEGATE_RetVal(FTransform, FGetHallPortalDestDelegate);

/** Base room manager class for halls between rooms. */
UCLASS(Abstract, NotBlueprintable)
class AHallRoomManager : public ARoomManager
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * Initialize the hall manager.
	 * 
	 * @param RoomSpawnInfo Generated information about the room.
	 * @param BoxExtents	Extents to use for the room's activation hitbox.
	 * @param bPortal		Whether to spawn a portal when the hall is completed.
	 */
	void Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents, const bool bPortal = false);

	/** Get the transform of a portal exiting this hall. */
	UE_NODISCARD FTransform GetPortalSource() const;

	/** Get the transform of a portal entering this hall. */
	UE_NODISCARD FTransform GetPortalDestination() const;

	/**
	 * Delegate that will get the transform of the exit portal connected to a portal leaving this hall.
	 * 
	 * @return	Transform of the exit portal connected to a portal leaving this hall.
	 */
	FGetHallPortalDestDelegate GetPortalDestDelegate;

protected:
	//~  Begin ARoomManager Interface.
	virtual void OnEnabled() override;
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	virtual void OnCompletion() override;
	//~ End ARoomManager Interaface

	/** Spawn a portal connecting this hall to an arena. */
	void SpawnPortal();

	/** Event called when an actor overlaps the exit hitbox. */
	UFUNCTION()
	void OnExitBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Blueprint class that manages static meshes for the hall. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<AHallMeshManager> MeshManagerClass;

	/** Manages static meshes for the hall. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AHallMeshManager> MeshManager;

	/** Blueprint class that manages static meshes for underground halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<APortalHallMeshManager> PortalMeshManagerClass;

	/** Manages static meshes for underground halls. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<APortalHallMeshManager> PortalMeshManager;

	/** Hitbox for the hall's exit that handles room completion when activated. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> ExitCollision;

	/** Whether to automatically handle completion, or allow subclasses to handle it. */
	bool bHandleCompletion = false;
};
