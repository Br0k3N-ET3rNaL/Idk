// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomSpawnInfo.h"
#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/UniquePtr.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "RoomTree.generated.h"

enum class EDataValidationResult : uint8;
class AArenaRoomManager;
class AHallRoomManager;
class FDataValidationContext;
class FMapPartitionBounds;
class UIdkRandomStream;
class URoom;
class UArenaNode;

UENUM()
enum class EPathTaken : uint8
{
	LeftHall,
	RightHall,
	/** Arena was completed. */
	Arena,
};

/** Represents a hall between arenas in the room tree. */
UCLASS()
class UHallNode final : public UObject
{
	GENERATED_BODY()

	friend class URoomTree;
	friend class UArenaNode;

public:
	/** Set the hall's room manager. */
	void SetRoomManager(AHallRoomManager& InRoomManager);

	/** Get the hall's room manager. */
	UE_NODISCARD AHallRoomManager& GetRoomManager() const;

	/** Get the room manager for the room that the hall leads to. */
	UE_NODISCARD AArenaRoomManager& GetNextRoomManager() const;

	/** Check if the hall is an undeground hall connected by portals. */
	UE_NODISCARD bool IsPortal() const;

	/** Get the hall's spawn info. */
	UE_NODISCARD const FRoomSpawnInfo& GetRoomSpawnInfo() const;

	/** Free memory used by the hall's spawn info. */
	void ReleaseRoomSpawnInfo();

private:
	/** Initialize the hall with the specified spawn info. */
	void Init(FRoomSpawnInfo& InRoomSpawnInfo);

	/** Recursively destroy subobjects before the room tree is destroyed. */
	void StartCleanup();

	/** Node representing the arena the hall leads to. */
	UPROPERTY()
	TObjectPtr<UArenaNode> NextRoomNode;

	/** Information used to spawn the hall. */
	TUniquePtr<FRoomSpawnInfo> RoomSpawnInfo;

	/** Arena manager for the hall. */
	UPROPERTY()
	TObjectPtr<AHallRoomManager> RoomManager;

	/** Whether the path is an undeground hall connected by portals. */
	bool bPortal = false;
};

/** Represents an arena in the room tree. */
UCLASS()
class UArenaNode final : public UObject
{
	GENERATED_BODY()

	friend class URoomTree;
	friend class UHallNode;

public:
	/** Set the arena's room manager. */
	void SetRoomManager(AArenaRoomManager& InRoomManager);

	/** Get the arena's room manager. */
	UE_NODISCARD AArenaRoomManager& GetRoomManager() const;

	/** Get the left path. */
	UE_NODISCARD UHallNode* GetLeftHall() const;

	/** Get the right path. */
	UE_NODISCARD UHallNode* GetRightHall() const;

	/** Get the arena's spawn info. */
	UE_NODISCARD const FRoomSpawnInfo& GetRoomSpawnInfo() const;

	/** Free memory used by the arena's spawn info. */
	void ReleaseRoomSpawnInfo();

private:
	/** Initialize the arena with the specified spawn info. */
	void Init(FRoomSpawnInfo& InRoomSpawnInfo);

	/** Recursively destroy subobjects before the room tree is destroyed. */
	void StartCleanup();

	/** Left path exiting the arena. */
	UPROPERTY()
	TObjectPtr<UHallNode> LeftHall;

	/** Right path exiting the arena. */
	UPROPERTY()
	TObjectPtr<UHallNode> RightHall;

	/** Information used to spawn the arena. */
	TUniquePtr<FRoomSpawnInfo> RoomSpawnInfo;

	/** Arena manager for the arena. */
	UPROPERTY()
	TObjectPtr<AArenaRoomManager> RoomManager;
};

/** Used to generate the structure of the map. */
UCLASS(AutoExpandCategories = ("Room Tree", "Room Tree|Generation"))
class URoomTree final : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the room tree.
	 * 
	 * @param Seed					Seed used for random generation. 
	 * @param VoxelSize				Size of a voxel in unreal units.
	 * @param RightHallAngle		Angle (yaw) in radians of the right hall exiting an arena. 
	 * @param OutArenaNodes			Generated arena nodes.
	 * @param OutArenaHallBounds	Bounds for generated arenas and halls.
	 */
	void Init(const int32 Seed, const double VoxelSize, TArray<UArenaNode*>& OutArenaNodes, TArray<FMapPartitionBounds*>& OutArenaHallBounds);

	/**
	 * Set parameters that have been calculated externally.
	 * 
	 * @param InArenaRadius		Calculated radius of an arena. 
	 * @param InOuterHallLength Calculated length of the outer edge of a hall.
	 * @param InInnerHallLength Calculated length of the center of a hall.
	 * @param InHallWidth		Calculated width of a hall.
	 */
	void SetCalculatedParameters(const double InArenaRadius, const double InOuterHallLength, const double InInnerHallLength, const double InHallWidth, const double InRightHallAngle);

	/** Get the bounds of the room tree. */
	UE_NODISCARD FBox GetBounds() const;

	/** Get the inner extents for arenas. */
	UE_NODISCARD FVector GetArenaExtents() const;

	/** Get the inner extents for halls. */
	UE_NODISCARD FVector GetHallExtents() const;

	/** Get the radius of an arena. */
	UE_NODISCARD double GetRadius() const;

	/** Get the minimum distance between two arenas in the same row. */
	UE_NODISCARD double GetMinArenaSpacing() const;

	/** Get the (uncalculated) length of a hall. */
	UE_NODISCARD double GetHallLength() const;

	/** Get the (uncalculated) width of a hall. */
	UE_NODISCARD double GetHallWidth() const;

	/**
	 * Simulate the player's progress through the room tree.
	 * 
	 * @param PlayerProgress	Saved player progress. 
	 * @param OutPlayerPos		Position of the last room/path that the player completed.
	 */
	void SimulatePlayerProgress(const TArray<EPathTaken>& PlayerProgress, FVector& OutPlayerPos);

	/** Recursively destroy subobjects before the room tree is destroyed. */
	void StartCleanup();
	
private:
	/** Information used when setting up halls. */
	struct FHallSetupInfo final
	{
	public:
		/** Inner radius of an arena squared (in voxels). */
		double InnerRadiusSq = 0.0;

		/** Outer radius of an arena squared (in voxels). */
		double OuterRadiusSq = 0.0;

		/** Width of a hall (without the edge) squared (in voxels).  */
		double InnerHallWidthSq = 0.0;

		/** Width of a hall (with the edge) squared (in voxels).  */
		double OuterHallWidthSq = 0.0;

		/** Half the length of a hall in voxels. */
		double HalfHallLengthVoxel = 0.0;

		/** Half the width of a hall (without the edge) in voxels. */
		double HalfHallWidthVoxel = 0.0;

		/** Height of the interior of underground halls in voxels. */
		double PortalHallHeightVoxel = 0.0;

		/** Offset of an underground hall from the room it exits (in voxels). */
		FVector PortalHallOffset = FVector::ZeroVector;
	};

	/** Information used when generating the room tree. */
	struct FRoomTreeInfo final
	{
	public:
		/**
		 * @param CenterSpacingX	Distance between the center of two arenas (in voxels).
		 * @param CenterSpacingY	Distance between the center of two arenas in the same row (in voxels).
		 * @param ArenaBoundsOffset	Added/subtracted to/from an arena's position to get the max/min positions for that arena
		 * @param LateRowThreshold	Row where generation becomes more challenging. 
		 * @param PreBossRow		Row of the arenas right before a boss. 
		 */
		FRoomTreeInfo(const double CenterSpacingX, const double CenterSpacingY, const FVector& ArenaBoundsOffset, const uint8 LateRowThreshold, const uint8 PreBossRow);

		/** Increment the current width. */
		void IncrementWidth();

		/** Decrement the current width. */
		void DecrementWidth();

		/**
		 * Update the minimum and maximum positions based on the specified arena position.
		 * 
		 * @param ArenaCenter	Center of the arena to use. 
		 */
		void UpdateMinAndMax(const FVector& ArenaCenter);

		/** 
		 * Add an empty row of arenas. 
		 * 
		 * @param bIncrementRow	Whether to increase the row counter.
		 */
		void AddRow(const bool bIncrementRow = true);

		/** Add the specified arena to the current row of arenas. */
		void AddArena(UArenaNode& Arena);

		/** Add the specified path to the current row of halls. */
		void AddHall(UHallNode& Hall);

		/** Get the X position for the center of the current arena. */
		UE_NODISCARD double GetXForCurrentArena() const;

		/** Get the Y position for the center of the current arena. */
		UE_NODISCARD double GetYForCurrentArena() const;

		/**
		 * Check whether a hall should be created based on the current row and column.
		 * 
		 * @param bLeft	Whether to check for a left or right hall. 
		 * @return		True if a hall should be created, otherwise false. 
		 */
		UE_NODISCARD bool ShouldCreateHall(const bool bLeft) const;

		/** Connects the specified arena to the halls which enter it. */
		void ConnectArenaToPreviousHalls(UArenaNode& Arena);

		/** Distance between the center of two rows (in voxels). */
		const double CenterSpacingX;

		/** Distance between the center of two arenas in the same row (in voxels). */
		const double CenterSpacingY;

		/** Added/subtracted to/from an arena's position to get the max/min positions for that room. */
		const FVector ArenaBoundsOffset;

		/** Row where generation becomes more challenging.  */
		const uint8 LateRowThreshold;

		/** Row of the arenas right before a boss.  */
		const uint8 PreBossRow;

		/** Current row. */
		uint8 Row = 0;

		/** Current column in the current row. */
		uint8 Column = 0;

		/** Number of columns in the current row. */
		uint8 Width = 1;

		/** Number of columns in the previous row. */
		uint8 PrevWidth = 0;

		/** Current minimum position within the room tree. */
		FVector Min = FVector::ZeroVector;

		/** Current maximum position within the room tree. */
		FVector Max = FVector::ZeroVector;

		/** Outer array contains rows, inner array contains arenas within that row. */
		TArray<TArray<UArenaNode*>> Arenas;

		/** Outer array contains rows, inner array contains halls within that row. */
		TArray<TArray<UHallNode*>> Halls;

	};

	/**
	 * Randomly generate a type for an arena.
	 *
	 * @param Seed			Seed to use for random generation.
	 * @param RoomTreeInfo	Information used to generate the arena type.	
	 * @return				The generated arena type.
	 */
	UE_NODISCARD FGameplayTag GenerateArenaType(const int32 Seed, const FRoomTreeInfo& RoomTreeInfo) const;

	/**
	 * Randomly generate a type for a hall.
	 *
	 * @param Seed			Seed to use for random generation.
	 * @param RoomTreeInfo	Information used to generate the hall type.
	 * @param PrevArenaType	Type of the arena that the hall exits.
	 * @param bLeft			Whether the hall exits the previous arena from the left or right side.
	 * @return				The generated hall type.
	 */
	UE_NODISCARD FGameplayTag GenerateHallType(const int32 Seed, const FRoomTreeInfo& RoomTreeInfo, const FGameplayTag PrevArenaType, const bool bLeft) const;

	/**
	 * Finish setting up the specified path.
	 * 
	 * @param Hall					Hall to finish setting up.
	 * @param PrevArena				Arena the hall exits.
	 * @param HallSetupInfo			Information used to setup the hall.		
	 * @param InOutMin				Current minimum position in the room tree.
	 * @param OutArenaHallBounds	Bounds for generated arenas and paths.
	 */
	void FinishHallSetup(UHallNode& Hall, UArenaNode& PrevArena, const FHallSetupInfo& HallSetupInfo, FVector& InOutMin, TArray<FMapPartitionBounds*>& OutArenaHallBounds);

	/**
	 * Create and initialize an arena.
	 * 
	 * @param Rng			Random number generator. 
	 * @param RoomTreeInfo	Information used to create the arena.
	 * @param FixedType		If set, determines the arenas's type, otherwise, type is generated randomly.
	 * @return				The newly created arena.
	 */
	UE_NODISCARD UArenaNode& CreateArena(UIdkRandomStream& Rng, FRoomTreeInfo& RoomTreeInfo, const FGameplayTag& FixedType = FGameplayTag()) const;

	/**
	 * Create and initialize a hall.
	 * @param Rng				Random number generator.
	 * @param RoomTreeInfo		Information used to create the hall.	
	 * @param PreviousRoomType	Type of the arena that the hall exits.
	 * @param bLeft				Whether the hall exits the previous arena from the left or right side.
	 * @return					The newly created path.
	 */
	UE_NODISCARD UHallNode& CreateHall(UIdkRandomStream& Rng, FRoomTreeInfo& RoomTreeInfo, const FGameplayTag& PreviousRoomType, const bool bLeft) const;

	/** Number of paths exiting each room. @note Arenas before the boss will only have one path each. */
	const uint8 HallsPerArena = 2;

	/** Maximum number of arenas per row. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Structure Parameters", meta = (ClampMin = 2))
	uint8 MaxWidth = 4;

	/** Number of max width rows to have. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Structure Parameters", meta = (ClampMin = 1))
	uint8 MaxWidthMaxLength = 1;

	/** Minimum value for the scaled distance of an arena or hall from the edge of the map. Used when generating arena and hall types. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (ClampMin = 0.01, ClampMax = 1.0))
	double MinScaledDistance = 0.25;

	/** Maximum chance of a horde room being generated in early arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 EarlyHordeMaxChance = 30;

	/** Maximum chance of a horde room being generated in later arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 HordeMaxChance = 40;

	/** Maximum chance of a elite room being generated in early arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 EarlyEliteMaxChance = 0;

	/** Maximum chance of a elite room being generated in later arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 EliteMaxChance = 40;

	/** Maximum chance of a shop room being generated in early halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 EarlyShopMaxChance = 20;

	/** Maximum chance of a cursed room being generated in early halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 EarlyCurseMaxChance = 30;

	/** Maximum chance of a shop room being generated in later halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 ShopMaxChance = 30;

	/** Maximum chance of a cursed room being generated in later halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Room Chances", meta = (Units = "%", ClampMax = 100))
	uint8 CurseMaxChance = 40;

	/** Distance between the ground and the base of an arena (voxels). */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (ClampMin = 0.0))
	double ArenaBaseVoxelHeight = 1.0;

	/** Radius of an arena. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double ArenaRadius = 1000.0;

	/** Minimum distance between arenas in the same row. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double MinArenaSpacing = 200.0;

	/** Length of halls between arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double HallLength = 500.0;

	/** Width of halls between arenas. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double HallWidth = 500.0;

	/** Width of the edges of arenas and halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double EdgeWidth = 200.0;

	/** Z coordinate of underground halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double PortalHallDepth = -500.0;

	/** Height of the interior of underground halls. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double PortalHallHeight = 500.0;

	/** Externally calculated radius for arenas. */
	UPROPERTY(VisibleInstanceOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double CalculatedArenaRadius = 0.0;

	/** Externally calculated length of the outer edge of a hall.  */
	UPROPERTY(VisibleInstanceOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double CalculatedOuterHallLength = 0.0;

	/** Externally calculated length of the center of a hall. */
	UPROPERTY(VisibleInstanceOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double CalculatedInnerHallLength = 0.0;

	/** Externally calculated width of a path. */
	UPROPERTY(VisibleInstanceOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "cm"))
	double CalculatedHallWidth = 0.0;

	/** Angle of the front right hall. Used to calculate the angles of other halls. */
	UPROPERTY(VisibleInstanceOnly, Category = "Room Tree|Generation|Sizes", meta = (Units = "Radians"))
	double RightHallAngle = 0.0;

	/** Whether a fixed arena type should be used. */
	UPROPERTY(meta = (InlineEditConditionToggle))
	bool bOverrideArenaType = false;

	/** All arenas will be generated with this type. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation", meta = (EditCondition = "bOverrideArenaType", Categories = "Room.Arena"))
	FGameplayTag ArenaTypeOverride;

	/** Whether a fixed hall type should be used. */
	UPROPERTY(meta = (InlineEditConditionToggle))
	bool bOverrideHallType = false;

	/** All halls will be generated with this type. */
	UPROPERTY(EditDefaultsOnly, Category = "Room Tree|Generation", meta = (EditCondition = "bOverrideHallType", Categories = "Room.Hall"))
	FGameplayTag HallTypeOverride;

	/** Node for the spawn room. */
	UPROPERTY()
	TObjectPtr<UArenaNode> SpawnRoomNode;

	/** Node for the boss room. */
	UPROPERTY()
	TObjectPtr<UArenaNode> BossRoomNode;

	/** Bounds of the room tree. */
	FBox Bounds;

	/** Inner extents of arenas. */
	FVector ArenaExtents;

	/** Inner extents of halls. */
	FVector HallExtents;

};
