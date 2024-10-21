// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SharedPointer.h>
#include <Templates/UniquePtr.h>
#include <UObject/ObjectMacros.h>
#include <VoxelGenerators/VoxelGenerator.h>
#include <VoxelGenerators/VoxelGeneratorHelpers.h>
#include <VoxelGenerators/VoxelGeneratorInstance.h>
#include <VoxelMacros.h>
#include <VoxelMaterial.h>
#include <VoxelRange.h>
#include <VoxelSharedPtr.h>

#include "IdkVoxelGenerator.generated.h"

struct FVoxelGeneratorInit;
struct FVoxelIntBox;
struct FVoxelItemStack;

/** Base class for bounds used by the map partition. */
class FMapPartitionBounds
{
public:
	virtual ~FMapPartitionBounds() = 0;

	/**
	 * Get the value for the specified voxel. 
	 * 
	 * @param Point	Coordinates of the voxel to get the value for. 
	 * @return		Value for the specified voxel. 
					A positive value corresponds to an empty voxel, 
					and a negative value corresponds to a full voxel. 	
	 */
	UE_NODISCARD virtual double GetVoxelValue(const FVector& Point) const = 0;

	/** Get the maximum X value that is within the bounds. */
	UE_NODISCARD virtual double GetRightmostX() const = 0;

	/** Get the maximum Y value that is within the bounds. */
	UE_NODISCARD virtual double GetTopmostY() const = 0;

	/** Get the maximum Z value that is within the bounds. */
	UE_NODISCARD virtual double GetHighestZ() const = 0;

	/** Get the bounding box. */
	UE_NODISCARD FBox GetBounds() const;

protected:
	/** Bounding box. */
	FBox Bounds;
};

/** Bounds for underground halls connected by portals. */
class FPortalHallBounds final : public FMapPartitionBounds
{
public:
	/**
	 * @param Center			Position in the center of the base of the hall.
	 * @param HalfHallLength	Half the length of the hall.
	 * @param HalfHallWidth		Half the width of the hall.
	 * @param HallHeight		Height of the hall.
	 */
	FPortalHallBounds(const FVector& Center, const double HalfHallLength, const double HalfHallWidth, const double HallHeight);

	//~ Begin FMapPartitionBounds Interface.
	UE_NODISCARD virtual double GetVoxelValue(const FVector& Point) const override;
	UE_NODISCARD virtual double GetRightmostX() const override;
	UE_NODISCARD virtual double GetTopmostY() const override;
	UE_NODISCARD virtual double GetHighestZ() const override;
	//~ End FMapPartitionBounds Interface

};

/** Bounds for two arenas connected by a hall. */
class FArenaHallBounds final : public FMapPartitionBounds
{
public:
	/**
	 * @param CenterA			Center of the first arena. 
	 * @param CenterB			Center of the second arena.
	 * @param InnerRadiusSq		Squared radius of the inner edge of the arena.
	 * @param OuterRadiusSq		Squared radius of the outer edge of the arena.
	 * @param InnerHallWidthSq	Squared width of the inner edge of the hall.
	 * @param OuterHallWidthSq	Squared width of the outer edge of the hall.
	 */
	FArenaHallBounds(const FVector& CenterA, const FVector& CenterB, const double InnerRadiusSq, const double OuterRadiusSq, const double InnerHallWidthSq, const double OuterHallWidthSq);

	//~ Begin FMapPartitionBounds Interface.
	UE_NODISCARD virtual double GetVoxelValue(const FVector& Point) const override;
	UE_NODISCARD virtual double GetRightmostX() const override;
	UE_NODISCARD virtual double GetTopmostY() const override;
	UE_NODISCARD virtual double GetHighestZ() const override;
	//~ End FMapPartitionBounds Interface

private:
	/**
	 * Get the squared distance between a point and a line.
	 * 
	 * @param LineEndA		First endpoint of the line. 
	 * @param LineEndB		Second endpoint of the line.
	 * @param Point			Point to check.
	 * @return				Distance between the point and the line, 
							or if the point is beyond the endpoints returns -1.0.
	 */
	UE_NODISCARD static double DistanceSqFromPointToLine(const FVector2D& LineEndA, const FVector2D& LineEndB, const FVector2D& Point);

	/** Center of the first arena. */
	FVector2D PointA;

	/** Center of the second arena. */
	FVector2D PointB;

	/** Radius of the outer edge of the arenas. */
	double OuterRadius = 0.0;

	/** Squared radius of the inner edge of the arenas. */
	double InnerRadiusSq = 0.0;

	/** Squared radius of the outer edge of the arenas. */
	double OuterRadiusSq = 0.0;

	/** Difference between OuterRadiusSq and InnerRadiusSq */
	double RadiusSqDifference = 0.0;

	/** Squared width of the inner edge of the hall. */
	double InnerHallWidthSq = 0.0;

	/** Squared width of the outer edge of the hall. */
	double OuterHallWidthSq = 0.0;

	/** Difference between OuterHallWidthSq and InnerHallWidthSq. */
	double WidthSqDifference = 0.0;

	/** Z coordinate of the arenas. */
	double Height = 0.0;

};

/** Node within a map partition. */
class FMapPartitionNode final
{
public:
	/**
	 * Initialize the map partition node.
	 * 
	 * @param Objects	Objects to partition. 
	 * @param bInUseX	Whether to split on X or Y.
	 */
	void Init(TArray<FMapPartitionBounds*>& Objects, const bool bInUseX = false);

	/** Get the bounds of this node. */
	UE_NODISCARD FBox GetBounds() const;

	/**
	 * Get the value for the specified voxel.
	 *
	 * @param Point	Coordinates of the voxel to get the value for.
	 * @return		Value for the specified voxel.
					A positive value corresponds to an empty voxel,
					and a negative value corresponds to a full voxel.
	 */
	UE_NODISCARD double GetVoxelValue(const FVector& Point) const;

protected:
	/** Whether this node is a leaf node. */
	bool bLeaf = false;

	/** Whether this node is split on X or Y. */
	bool bUseX = true;

	/** Center coordinate for subobjects. Either X or Y depending on bUseX. */
	double CenterCoord = 0.0;

	/** Bounds of this node. */
	FBox Bounds;

	/** Nodes less than CenterCoord. */
	TUniquePtr<FMapPartitionNode> LesserHalf;

	/** Nodes greater than CenterCoord. */
	TUniquePtr<FMapPartitionNode> GreaterHalf;

	/** Object for leaf nodes. */
	TUniquePtr<FMapPartitionBounds> LeafObject;

};

/** Map partition. */
class FMapPartition final
{
public:
	/**
	 * Initialize the partition.

	 * @param Objects	Objects to partition. 
	 */
	void Init(TArray<FMapPartitionBounds*>& Objects);

	/**
	 * Get the value for the specified voxel.
	 *
	 * @param Point	Coordinates of the voxel to get the value for.
	 * @return		Value for the specified voxel.
					A positive value corresponds to an empty voxel,
					and a negative value corresponds to a full voxel.
	 */
	UE_NODISCARD double GetVoxelValue(const FVector& Point) const;

private:
	/** Root of the partition */
	TUniquePtr<FMapPartitionNode> Root;

};

UCLASS(Blueprintable)
class UIdkVoxelGenerator final : public UVoxelGenerator
{
	GENERATED_BODY()
	
public:
	//~ Begin UVoxelGenerator Interface.
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface

	/** Map partition. */
	TSharedPtr<FMapPartition> Map;

};

/** Generates terrain based on supplied map partition. */
class FIdkVoxelGeneratorInstance final : public TVoxelGeneratorInstanceHelper<FIdkVoxelGeneratorInstance, UIdkVoxelGenerator>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FIdkVoxelGeneratorInstance, UIdkVoxelGenerator>;

	explicit FIdkVoxelGeneratorInstance(const UIdkVoxelGenerator& Generator);

	//~ Begin FVoxelGeneratorInstance Interface
	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;
	//~ End FVoxelGeneratorInstance Interface

private:
	/** Map partition. */
	TSharedPtr<FMapPartition> Map;

};
