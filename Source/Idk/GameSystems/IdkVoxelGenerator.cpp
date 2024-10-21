// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/IdkVoxelGenerator.h"

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Math/Color.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SharedPointer.h>
#include <Templates/UniquePtr.h>
#include <VoxelEnums.h>
#include <VoxelGenerators/VoxelGeneratorInit.h>
#include <VoxelGenerators/VoxelGeneratorInstance.h>
#include <VoxelIntBox.h>
#include <VoxelItemStack.h>
#include <VoxelMacros.h>
#include <VoxelMaterial.h>
#include <VoxelMaterialBuilder.h>
#include <VoxelRange.h>
#include <VoxelSharedPtr.h>

FMapPartitionBounds::~FMapPartitionBounds() {}

FBox FMapPartitionBounds::GetBounds() const
{
	return Bounds;
}

FPortalHallBounds::FPortalHallBounds(const FVector& Center, const double HalfHallLength, const double HalfHallWidth, const double HallHeight)
{
	FVector Min = Center - FVector(HalfHallLength, HalfHallWidth, 0.0);
	FVector Max = Center + FVector(HalfHallLength, HalfHallWidth, HallHeight);

	for (uint8 i = 0; i < 3; ++i)
	{
		Min[i] = FMath::FloorToDouble(Min[i]);
		Max[i] = FMath::CeilToDouble(Max[i]);
	}

	Bounds = FBox(Min, Max);
}

double FPortalHallBounds::GetVoxelValue(const FVector& Point) const
{
	if (Bounds.IsInsideOrOn(Point))
	{
		return 1.0;
	}

	return Point.Z;
}

double FPortalHallBounds::GetRightmostX() const
{
	return Bounds.Max.X;
}

double FPortalHallBounds::GetTopmostY() const
{
	return Bounds.Max.Y;
}

double FPortalHallBounds::GetHighestZ() const
{
	return Bounds.Max.Z;
}

FArenaHallBounds::FArenaHallBounds(const FVector& CenterA, const FVector& CenterB, const double InnerRadiusSq, const double OuterRadiusSq, const double InnerHallWidthSq, const double OuterHallWidthSq)
	: PointA(CenterA), PointB(CenterB), InnerRadiusSq(InnerRadiusSq), OuterRadiusSq(OuterRadiusSq), InnerHallWidthSq(InnerHallWidthSq), OuterHallWidthSq(OuterHallWidthSq)
{
	Height = CenterA.Z;

	RadiusSqDifference = OuterRadiusSq - InnerRadiusSq;

	check(OuterHallWidthSq <= OuterRadiusSq);

	WidthSqDifference = OuterHallWidthSq - InnerHallWidthSq;

	OuterRadius = FMath::Sqrt(OuterRadiusSq);

	const FVector BoundsOffset = FVector(OuterRadius, OuterRadius, 0.0);
	const FVector MinHeightOffset = FVector(0.0, 0.0, Height);
	const FVector Min = FVector::Min(CenterA, CenterB) - BoundsOffset - MinHeightOffset;
	const FVector Max = FVector::Max(CenterA, CenterB) + BoundsOffset;

	Bounds = FBox(Min, Max);
}

double FArenaHallBounds::GetVoxelValue(const FVector& Point) const
{
	const double Z = Point.Z;

	if (Z <= Height)
	{
		const FVector2D Point2D = FVector2D(Point);
		const double DistSqA = FVector2D::DistSquared(PointA, Point2D);
		const double DistSqB = FVector2D::DistSquared(PointB, Point2D);

		// Check if the point is within the inner radius of one of the rooms
		if (DistSqA <= InnerRadiusSq || DistSqB <= InnerRadiusSq)
		{
			return Z - Height;
		}
		else
		{
			const double LineDistSq = DistanceSqFromPointToLine(PointA, PointB, Point2D);

			// Check if the point is within the inner width of the path
			if (LineDistSq >= 0.0 && LineDistSq <= InnerHallWidthSq)
			{
				return Z - Height;
			}
			else
			{
				double CircleHeight = 0.0;
				double PathHeight = 0.0;

				// Check if the point is within the outer radius of one of the rooms
				if (DistSqA <= OuterRadiusSq || DistSqB <= OuterRadiusSq)
				{
					// Between 0.0 and 1.0 depending on how close the point is to the outer edge
					CircleHeight = (OuterRadiusSq - FMath::Min(DistSqA, DistSqB)) / RadiusSqDifference;
				}

				// Check if the point is within the outer width of the path
				if (LineDistSq >= 0.0 && LineDistSq <= OuterHallWidthSq)
				{
					// Between 0.0 and 1.0 depending on how close the point is to the outer edge
					PathHeight = (OuterHallWidthSq - LineDistSq) / WidthSqDifference;
				}

				return Z - FMath::Max(CircleHeight, PathHeight);
			}
		}
	}

	return Z;
}

double FArenaHallBounds::GetRightmostX() const
{
	return FMath::Max(PointA.X, PointB.X) + OuterRadius;
}

double FArenaHallBounds::GetTopmostY() const
{
	return FMath::Max(PointA.Y, PointB.Y) + OuterRadius;
}

double FArenaHallBounds::GetHighestZ() const
{
	return Height;
}

double FArenaHallBounds::DistanceSqFromPointToLine(const FVector2D& LineEndA, const FVector2D& LineEndB, const FVector2D& Point)
{
	const double DivisorSq = FMath::Square(LineEndB.X - LineEndA.X) + FMath::Square(LineEndB.Y - LineEndA.Y);

	if (DivisorSq > 0)
	{
		// Represents the parameter for the point parallel to the specified point on the parametric equation of the line
		const double T = FVector2D::DotProduct(Point - LineEndA, LineEndB - LineEndA) / DivisorSq;

		// Check if the point is in the area between the two endpoints
		if (T >= 0.0 && T <= 1.0)
		{
			return FVector2D::DistSquared(LineEndA + (T * (LineEndB - LineEndA)), Point);
		}
	}

	return -1.0;
}

void FMapPartitionNode::Init(TArray<FMapPartitionBounds*>& Objects, const bool bInUseX)
{
	bUseX = bInUseX;

	if (Objects.Num() == 1)
	{
		bLeaf = true;
		LeafObject = TUniquePtr<FMapPartitionBounds>(Objects.Top());
		Bounds = LeafObject->GetBounds();
	}
	else
	{
		Objects.Sort([&bUseX = bUseX](const FMapPartitionBounds& A, const FMapPartitionBounds& B)
			{
				if (bUseX)
				{
					return A.GetRightmostX() < B.GetRightmostX();
				}
				else
				{
					return A.GetTopmostY() < B.GetTopmostY();
				}
			});


		const int32 MidIndex = Objects.Num() / 2;

		if (bUseX)
		{
			CenterCoord = Objects[MidIndex - 1]->GetRightmostX();
		}
		else
		{
			CenterCoord = Objects[MidIndex - 1]->GetTopmostY();
		}

		// Split objects
		TArray<FMapPartitionBounds*> LesserObjects = TArray<FMapPartitionBounds*>(&Objects[0], MidIndex);
		TArray<FMapPartitionBounds*> GreaterObjects = TArray<FMapPartitionBounds*>(&Objects[MidIndex], Objects.Num() - MidIndex);

		Objects.Empty();

		Bounds.Init();

		if (!LesserObjects.IsEmpty())
		{
			LesserHalf = TUniquePtr<FMapPartitionNode>(new FMapPartitionNode);
			LesserHalf->Init(LesserObjects, !bUseX);
			Bounds += LesserHalf->GetBounds();
		}

		if (!GreaterObjects.IsEmpty())
		{
			GreaterHalf = TUniquePtr<FMapPartitionNode>(new FMapPartitionNode);
			GreaterHalf->Init(GreaterObjects, !bUseX);
			Bounds += GreaterHalf->GetBounds();
		}
	}
}

FBox FMapPartitionNode::GetBounds() const
{
	return Bounds;
}

double FMapPartitionNode::GetVoxelValue(const FVector& Point) const
{
	if (bLeaf)
	{
		return LeafObject->GetVoxelValue(Point);
	}
	else if (Bounds.IsInsideOrOn(Point))
	{
		double LesserVoxelValue = Point.Z;
		double GreaterVoxelValue = Point.Z;

		if (LesserHalf)
		{
			LesserVoxelValue = LesserHalf->GetVoxelValue(Point);
		}

		if (GreaterHalf)
		{
			GreaterVoxelValue = GreaterHalf->GetVoxelValue(Point);
		}

		if (Point.Z >= 0.0)
		{
			return FMath::Min(LesserVoxelValue, GreaterVoxelValue);
		}
		else
		{
			return FMath::Max(LesserVoxelValue, GreaterVoxelValue);
		}
	}

	return Point.Z;
}

void FMapPartition::Init(TArray<FMapPartitionBounds*>& Objects)
{
	Root = TUniquePtr<FMapPartitionNode>(new FMapPartitionNode);

	Root->Init(Objects);
}

double FMapPartition::GetVoxelValue(const FVector& Point) const
{
	return Root->GetVoxelValue(Point);
}

TVoxelSharedRef<FVoxelGeneratorInstance> UIdkVoxelGenerator::GetInstance()
{
	return MakeVoxelShared<FIdkVoxelGeneratorInstance>(*this);
}

FIdkVoxelGeneratorInstance::FIdkVoxelGeneratorInstance(const UIdkVoxelGenerator& Generator)
	: Super(&Generator)
{
	Map = TSharedPtr(Generator.Map);
}

FVector FIdkVoxelGeneratorInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	return FVector::UpVector;
}

v_flt FIdkVoxelGeneratorInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	// Positive value -> empty voxel
	// Negative value -> full voxel
	v_flt Value = Map->GetVoxelValue(FVector(X, Y, Z));

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5;

	return Value;
}

FVoxelMaterial FIdkVoxelGeneratorInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	v_flt Value = Map->GetVoxelValue(FVector(X, Y, Z));

	Builder.SetMaterialConfig(EVoxelMaterialConfig::RGB);
	Builder.SetColor(FLinearColor(1.f - (Z - Value), 0.f, 0.f, 0.f));

	return Builder.Build();
}

TVoxelRange<v_flt> FIdkVoxelGeneratorInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	//return TVoxelRange<v_flt>::Infinite();

	// Z can go from min to max
	TVoxelRange<v_flt> Value = TVoxelRange<v_flt>(Bounds.Min.Z, Bounds.Max.Z);

	Value /= 5;

	return Value;
}
