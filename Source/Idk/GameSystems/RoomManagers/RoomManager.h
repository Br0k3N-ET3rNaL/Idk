// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/UnrealString.h>
#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "RoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AArenaMeshManager;
class AIdkPlayerCharacter;
class APortal;
class FDataValidationContext;
class UBoxComponent;
class UChildActorComponent;
class UIdkRandomStream;
class UPrimitiveComponent;
class UArenaNode;
struct FHitResult;
struct FRoomSpawnInfo;

/** Base room manager class. */
UCLASS(Abstract, NotBlueprintable)
class ARoomManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/** Enables the room, allowing it to be activated by the player. */
	void Enable();

	/** Disables the room. */
	void Disable();

	/** Activate the room. */
	void Activate();

	/** Called to mark the room as completed. */
	void Complete();

	/** Delegate called when the room is completed. */
	FSimpleMulticastDelegate RoomCompletedDelegate;

	/** Delegate called when the room is activated. */
	FSimpleMulticastDelegate RoomActivatedDelegate;

protected:
	/** Event called when the room is activated. */
	virtual void OnEnabled();

	/** Event called when the room is disabled. */
	virtual void OnDisabled();

	/** Event called when the room is activated. */
	virtual void OnRoomActivated();

	/** Event called when the room is completed. */
	virtual void OnCompletion();

	/**
	 * Initialize the base room manager.
	 * 
	 * @param RoomSpawnInfo	Generated information about the room.
	 * @param BoxExtents	Extents to use for the room's activation hitbox.
	 */
	void Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents);

	/** Event called when the room's hitbox overlaps an actor. */
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Spawn a portal connecting two locations.
	 * 
	 * @param Src	Transform of the entrance portal.
	 * @param Dest	Transform of the exit portal.
	 */
	void SpawnPortal(const FTransform& Src, const FTransform& Dest);

	/** Blueprint class of the portal. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<APortal> PortalClass;

	/** Portal spawned to connect two rooms. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<APortal> Portal;

	/** Hitbox used to activate the room. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> BoxCollisionComp;

	/** Level of room. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 Level = 1;

	/** Seed used by the room for random generation. */
	UPROPERTY(VisibleInstanceOnly)
	int32 Seed = 0;

#if WITH_EDITORONLY_DATA
	/** Alphanumeric string representing the seed. */
	UPROPERTY(VisibleInstanceOnly)
	FString SeedString;
#endif

	/** Random number generator used by the room. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UIdkRandomStream> Rng;

	/** Whether to spawn a portal when the room is completed. */
	UPROPERTY(VisibleAnywhere)
	bool bSpawnPortal = false;

	/** Whether the room is currently enabled. */
	UPROPERTY(VisibleInstanceOnly)
	bool bEnabled = false;

	/** Whether the room is currently completed. */
	UPROPERTY(VisibleInstanceOnly)
	bool bComplete = false;

	/** Extents to use for the room's activation hitbox. */
	FVector BoxCollisionExtents;

	/** The current player. */
	TObjectPtr<AIdkPlayerCharacter> Player;

};
