// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/PimplPtr.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "MapManager.generated.h"

enum class EDataValidationResult : uint8;
enum class EPathTaken : uint8;
class AArenaMeshManager;
class AArenaRoomManager;
class AHallMeshManager;
class AIdkCharacter;
class AIdkPlayerCharacter;
class ANavMeshBoundsVolume;
class AVoxelWorld;
class FDataValidationContext;
class UBossEncounters;
class UCurveFloat;
class UEliteEncounters;
class UIdkGameInstance;
class UIdkRandomStream;
class UMapWidget;
class UMaterialInterface;
class UMidRunSaveGame;
class URoomManagerClasses;
class URoomTree;
class UArenaNode;
class UHallNode;
class USceneCaptureComponent2D;
class USpawnableEnemyClasses;
class UStaticMesh;
class UStaticMeshComponent;
struct FMapInitInfo;

/** Manages the game map. Handles room generation and terrain generation. */
UCLASS()
class AMapManager final : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	//~ End AActor Interface

	/** Set the current player class. */
	void SetPlayerClass(TSubclassOf<AIdkPlayerCharacter> InPlayerClass);

	/** Set the save to load from. */
	void LoadFromSave(const UMidRunSaveGame& InSave);

	/** Delegate called when the player is spawned. */
	FSimpleDelegate OnPlayerSpawnedDelegate;

private:
	/**
	 * Saves the player's progress through the map.
	 * 
	 * @param PathTaken	Represents the path the player took. 
	 */
	void UpdatePlayerProgress(const EPathTaken PathTaken);

	/** 
	 * Initialize static variables for the mesh managers. 
	 * 
	 * @param OutHallWidth	Width of a hall (in cm).
	 */
	void InitializeMeshManagers(double& OutHallWidth);

	/** Initialize the size for the voxel world. */
	void InitializeVoxelWorldSize();

	/**
	 * Initialize the map.
	 *
	 * @param Bounds	Bounding box for the world (in cm). 
	 * @param HallWidth Width of a hall (in cm).
	 */
	void InitializeMap(const FBox& Bounds, const double HallWidth);

	/**
	 * Spawn the planes that prevents light from reaching underground .
	 * 
	 * @param Bounds	Bounding box for the world (in cm). 
	 */
	void SpawnPlanes(const FBox& Bounds);

	/**
	 * Spawn a room manager for an arena.
	 * 
	 * @param Arena			Node in the room tree representing an arena to spawn a room manager for.
	 * @param ArenaExtents	Extents for the arena.
	 * @param HallExtents	Extents for halls. 
	 */
	void SpawnArenaRoomManager(UArenaNode& Arena, const FVector& ArenaExtents, const FVector& HallExtents);

	/**
	 * Spawn a room manager for a hall.
	 * 
	 * @param Hall					Node in the room tree representing a hall to spawn a room manager for.
	 * @param PrevArenaRoomManager	Room manager for the arena the hall exits.
	 * @param HallExtents			Extents for the hall.
	 * @param bLeft					Whether the hall exits the previous arena from the left or right side.
	 */
	void SpawnHallRoomManager(UHallNode& Hall, AArenaRoomManager& PrevArenaRoomManager, const FVector& HallExtents, const bool bLeft);

	/**
	 * Link a hall to the two arenas it connects.
	 * 
	 * @param Hall					Node in the room tree representing the hall to link.
	 * @param PrevArenaRoomManager	Room manager for the arena the hall exits.
	 * @param bLeft					Whether the hall exits the previous arena from the left or right side.
	 */
	void LinkHallToArenas(UHallNode& Hall, AArenaRoomManager& PrevArenaRoomManager, const bool bLeft);

	/** Spawn the player character. */
	UFUNCTION()
	void SpawnPlayer();

	/** Material used for the generated terrain. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UMaterialInterface> VoxelMaterial;

	/** Random number generator. */
	UPROPERTY(VisibleInstanceOnly, Instanced)
	TObjectPtr<UIdkRandomStream> Rng;

	/** Tree structure containing all the generated rooms. */
	UPROPERTY(VisibleAnywhere, Instanced)
	TObjectPtr<URoomTree> RoomTree;

	/** Class that manages static meshes for arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings", meta = (BlueprintBaseOnly))
	TSoftClassPtr<AArenaMeshManager> ArenaMeshManagerClass;

	/** Class that manages static meshes for halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings", meta = (BlueprintBaseOnly))
	TSoftClassPtr<AHallMeshManager> HallMeshManagerClass;

	/** Size of a voxel in unreal units. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0"))
	double VoxelSize = 100.0;

	/** Data asset containing the classes of room managers for each room type. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<URoomManagerClasses> RoomManagerClasses;

	/** Contains information about all spawnable enemy types. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TObjectPtr<USpawnableEnemyClasses> SpawnableEnemyClasses;

	/** Contains all possible elite encounters that can be spawned. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TObjectPtr<UEliteEncounters> EliteEncounters;

	/** Contains all possible boss encounters that can be spawned. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TObjectPtr<UBossEncounters> BossEncounters;

	/** Curve that determines the budget that the spawner has to spawn enemies. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TObjectPtr<UCurveFloat> SpawnBudgetCurve;

	/** Static mesh representing a plane. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UStaticMesh> PlaneMesh;

	/** Class of the current player character. */
	UPROPERTY(VisibleInstanceOnly)
	TSoftClassPtr<AIdkPlayerCharacter> PlayerClass;

	/** Plane that prevents light from reaching underground. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UStaticMeshComponent> TopPlane;

	/** Plane that prevents light from reaching underground. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UStaticMeshComponent> BottomPlane;

	/** Voxel world that represents the map's terrain. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AVoxelWorld> VoxelWorld;

	/** Volume used to define the area to generate navmeshes for. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ANavMeshBoundsVolume> NavMeshVolume;

	/** Scene capture component used to generate the map widget. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneCaptureComponent2D> MapSceneCapture;

	/** Game instance. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UIdkGameInstance> GameInstance;

	/** Information used to initialize the map widget. */
	TPimplPtr<FMapInitInfo> MapInitInfo;

	/** Current save game. */
	TObjectPtr<const UMidRunSaveGame> Save;

	/** Whether player progress is currently begin simulated after loading a save. */
	bool bSimulatingPlayerProgress = false;

};
