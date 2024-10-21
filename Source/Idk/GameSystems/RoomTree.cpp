// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/RoomTree.h"

#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/IdkVoxelGenerator.h"
#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include "Idk/GameSystems/RoomManagers/RoomManager.h"
#include "Idk/GameSystems/RoomSpawnInfo.h"
#include "Idk/GameSystems/RoomTypes.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <CoreGlobals.h>
#include <GameplayTagContainer.h>
#include <GenericPlatform/GenericPlatformMath.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Logging/LogMacros.h>
#include <Math/MathFwd.h>
#include <Math/Range.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/UniquePtr.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

void UHallNode::SetRoomManager(AHallRoomManager& InRoomManager)
{
	RoomManager = &InRoomManager;
}

AHallRoomManager& UHallNode::GetRoomManager() const
{
	check(RoomManager);

	return *RoomManager;
}

AArenaRoomManager& UHallNode::GetNextRoomManager() const
{
	check(NextRoomNode);
	check(NextRoomNode->RoomManager);

	return *NextRoomNode->RoomManager;
}

bool UHallNode::IsPortal() const
{
	return bPortal;
}

const FRoomSpawnInfo& UHallNode::GetRoomSpawnInfo() const
{
	return *RoomSpawnInfo;
}

void UHallNode::ReleaseRoomSpawnInfo()
{
	RoomSpawnInfo.Reset();
}

void UHallNode::Init(FRoomSpawnInfo& InRoomSpawnInfo)
{
	RoomSpawnInfo = TUniquePtr<FRoomSpawnInfo>(&InRoomSpawnInfo);
}

void UHallNode::StartCleanup()
{
	if (RoomManager)
	{
		RoomManager->Destroy();
	}

	if (NextRoomNode)
	{
		NextRoomNode->StartCleanup();
	}
}

void UArenaNode::SetRoomManager(AArenaRoomManager& InRoomManager)
{
	RoomManager = &InRoomManager;
}

AArenaRoomManager& UArenaNode::GetRoomManager() const
{
	return *RoomManager;
}

UHallNode* UArenaNode::GetLeftHall() const
{
	return LeftHall;
}

UHallNode* UArenaNode::GetRightHall() const
{
	return RightHall;
}

const FRoomSpawnInfo& UArenaNode::GetRoomSpawnInfo() const
{
	return *RoomSpawnInfo;
}

void UArenaNode::ReleaseRoomSpawnInfo()
{
	RoomSpawnInfo.Reset();
}

void UArenaNode::Init(FRoomSpawnInfo& InRoomSpawnInfo)
{
	RoomSpawnInfo = TUniquePtr<FRoomSpawnInfo>(&InRoomSpawnInfo);
}

void UArenaNode::StartCleanup()
{
	if (RoomManager)
	{
		RoomManager->Destroy();
	}

	if (LeftHall)
	{
		LeftHall->StartCleanup();
	}
	
	if (RightHall)
	{
		RightHall->StartCleanup();
	}
}

#if WITH_EDITOR
EDataValidationResult URoomTree::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (EarlyHordeMaxChance + EarlyEliteMaxChance > 100)
	{
		Context.AddError(FText::FromString(TEXT("Early Horde Max Chance + Early Elite Max Chance should be at most 100.")));

		Result = EDataValidationResult::Invalid;
	}

	if (HordeMaxChance + EliteMaxChance > 100)
	{
		Context.AddError(FText::FromString(TEXT("Horde Max Chance + Elite Max Chance should be at most 100.")));

		Result = EDataValidationResult::Invalid;
	}

	if (EarlyShopMaxChance + EarlyCurseMaxChance > 100)
	{
		Context.AddError(FText::FromString(TEXT("Early Shop Max Chance + Early Curse Max Chance should be at most 100.")));

		Result = EDataValidationResult::Invalid;
	}

	if (ShopMaxChance + CurseMaxChance > 100)
	{
		Context.AddError(FText::FromString(TEXT("Shop Max Chance + Curse Max Chance should be at most 100.")));

		Result = EDataValidationResult::Invalid;
	}

	if (bOverrideArenaType && (!ArenaTypeOverride.IsValid() || ArenaTypeOverride.MatchesTagExact(TAG_Room_Arena)))
	{
		Context.AddError(FText::FromString(TEXT("Arena Type Override is invalid.")));

		Result = EDataValidationResult::Invalid;
	}

	if (bOverrideHallType && (!HallTypeOverride.IsValid() || HallTypeOverride.MatchesTagExact(TAG_Room_Hall)))
	{
		Context.AddError(FText::FromString(TEXT("Hall Type Override is invalid.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void URoomTree::Init(const int32 Seed, const double VoxelSize, TArray<UArenaNode*>& OutArenaNodes, TArray<FMapPartitionBounds*>& OutArenaHallBounds)
{
	UIdkRandomStream& Rng = UIdkRandomStream::CreateRandomStream(GetOuter(), Seed);

	const double ArenaBoxExtent = (UE_DOUBLE_SQRT_2 / 2.0) * CalculatedArenaRadius;

	ArenaExtents = FVector(ArenaBoxExtent, ArenaBoxExtent, 2.0 * VoxelSize);

	HallExtents = FVector(CalculatedInnerHallLength / 2.0, CalculatedHallWidth / 2.0, 2.0 * VoxelSize);

	uint8 NumArenas = 0;

	for (uint8 i = HallsPerArena; i < MaxWidth; i++)
	{
		NumArenas += 2 * i;
	}

	NumArenas += MaxWidthMaxLength * MaxWidth;
	NumArenas += (MaxWidthMaxLength - 1) * (MaxWidth - 1);

	uint8 NumHalls = 2 * NumArenas;

	// Distance between the center of two arenas connected by a hall (in voxels)
	const double CenterSpacing = ((2.0 * CalculatedArenaRadius) + CalculatedInnerHallLength) / VoxelSize;

	// Outer radius of arenas (in voxels)
	const double RadiusScaled = (CalculatedArenaRadius + EdgeWidth) / VoxelSize;

	FRoomTreeInfo RoomTreeInfo = FRoomTreeInfo(
		FMath::Cos(RightHallAngle) * CenterSpacing,
		FMath::Sin(RightHallAngle) * CenterSpacing * 2.0,
		FVector(RadiusScaled, RadiusScaled, 0.0),
		MaxWidth - HallsPerArena + 1,
		((MaxWidth - HallsPerArena) * 2) + ((MaxWidthMaxLength * 2) - 1));

	// Current number of rows at max width
	uint8 MaxWidthLength = 0;

	// Whether we should generate less or more arenas for the current row
	bool bTaper = false;

	// Create the spawn room
	RoomTreeInfo.AddRow(false);
	SpawnRoomNode = CreateArena(Rng, RoomTreeInfo, TAG_Room_Spawn);
	OutArenaNodes.Add(SpawnRoomNode);

	RoomTreeInfo.IncrementWidth();

	TMap<FGameplayTag, uint8> TypeCounts;

	// Create rooms between the spawn and boss rooms
	do
	{
		RoomTreeInfo.AddRow();

		for (RoomTreeInfo.Column = 0; RoomTreeInfo.Column < RoomTreeInfo.Width; ++RoomTreeInfo.Column)
		{
			UArenaNode& NewArena = CreateArena(Rng, RoomTreeInfo);
			OutArenaNodes.Add(&NewArena);

			RoomTreeInfo.ConnectArenaToPreviousHalls(NewArena);
		}

		// Update the width
		if (RoomTreeInfo.Width == MaxWidth)
		{
			++MaxWidthLength;
			RoomTreeInfo.DecrementWidth();

			if (MaxWidthLength == MaxWidthMaxLength)
			{
				bTaper = true;
			}
		}
		else
		{
			if (bTaper)
			{
				RoomTreeInfo.DecrementWidth();
			}
			else
			{
				RoomTreeInfo.IncrementWidth();
			}
		}
	} 
	while (!bTaper || RoomTreeInfo.Width >= HallsPerArena);

	RoomTreeInfo.Column = 0;

	RoomTreeInfo.AddRow();
	BossRoomNode = CreateArena(Rng, RoomTreeInfo, TAG_Room_Boss);

	for (UHallNode* const Hall : RoomTreeInfo.Halls.Top())
	{
		Hall->NextRoomNode = BossRoomNode;
	}

	OutArenaNodes.Add(BossRoomNode);

	FHallSetupInfo HallSetupInfo;

	HallSetupInfo.InnerRadiusSq = FMath::Square(CalculatedArenaRadius / VoxelSize);
	HallSetupInfo.OuterRadiusSq = FMath::Square((CalculatedArenaRadius + EdgeWidth) / VoxelSize);
	HallSetupInfo.InnerHallWidthSq = FMath::Square((CalculatedHallWidth / 2.0) / VoxelSize);
	HallSetupInfo.OuterHallWidthSq = FMath::Square(((CalculatedHallWidth / 2.0) + EdgeWidth) / VoxelSize);

	HallSetupInfo.HalfHallLengthVoxel = (CalculatedOuterHallLength / 2.0) / VoxelSize;
	HallSetupInfo.HalfHallWidthVoxel = (CalculatedHallWidth / 2.0) / VoxelSize;
	HallSetupInfo.PortalHallHeightVoxel = PortalHallHeight / VoxelSize;
	HallSetupInfo.PortalHallOffset = FVector(HallSetupInfo.HalfHallLengthVoxel, 0.0, (PortalHallDepth - PortalHallHeight) / VoxelSize);

	for (uint8 i = 0; i < OutArenaNodes.Num() - RoomTreeInfo.Arenas.Top().Num(); ++i)
	{
		check(OutArenaNodes[i]);

		UArenaNode& Arena = *OutArenaNodes[i];

		if (UHallNode* LeftHall = Arena.LeftHall)
		{
			FinishHallSetup(*LeftHall, Arena, HallSetupInfo, RoomTreeInfo.Min, OutArenaHallBounds);
		}

		if (UHallNode* RightHall = Arena.RightHall)
		{
			FinishHallSetup(*RightHall, Arena, HallSetupInfo, RoomTreeInfo.Min, OutArenaHallBounds);
		}
	}

	Bounds = FBox(RoomTreeInfo.Min, RoomTreeInfo.Max);
}

void URoomTree::SetCalculatedParameters(const double InArenaRadius, const double InOuterHallLength, const double InInnerHallLength, const double InHallWidth, const double InRightHallAngle)
{
	CalculatedArenaRadius = InArenaRadius;
	CalculatedOuterHallLength = InOuterHallLength;
	CalculatedInnerHallLength = InInnerHallLength;
	CalculatedHallWidth = InHallWidth;
	RightHallAngle = InRightHallAngle;
}

FBox URoomTree::GetBounds() const
{
	return Bounds;
}

FVector URoomTree::GetArenaExtents() const
{
	return ArenaExtents;
}

FVector URoomTree::GetHallExtents() const
{
	return HallExtents;
}

double URoomTree::GetRadius() const
{
	return ArenaRadius;
}

double URoomTree::GetMinArenaSpacing() const
{
	return MinArenaSpacing;
}

double URoomTree::GetHallLength() const
{
	return HallLength;
}

double URoomTree::GetHallWidth() const
{
	return HallWidth;
}

void URoomTree::SimulatePlayerProgress(const TArray<EPathTaken>& PlayerProgress, FVector& OutPlayerPos)
{
	UArenaNode* CurrentArena = SpawnRoomNode;
	UHallNode* CurrentHall = nullptr;
	ARoomManager* LastRoomManager = CurrentArena->RoomManager;

	CurrentArena->RoomManager->Complete();

	check(PlayerProgress.IsEmpty() || PlayerProgress[0] != EPathTaken::Arena);

	for (const EPathTaken PathTaken : PlayerProgress)
	{
		LastRoomManager->Disable();

		if (PathTaken != EPathTaken::Arena)
		{
			if (PathTaken == EPathTaken::LeftHall)
			{
				CurrentHall = CurrentArena->LeftHall;
			}
			else
			{
				CurrentHall = CurrentArena->RightHall;
			}

			LastRoomManager = CurrentHall->RoomManager;
		}
		else
		{
			CurrentArena = CurrentHall->NextRoomNode;

			LastRoomManager = CurrentArena->RoomManager;
		}

		LastRoomManager->Complete();
	}

	OutPlayerPos = LastRoomManager->GetActorLocation();
}

void URoomTree::StartCleanup()
{
	if (SpawnRoomNode)
	{
		SpawnRoomNode->StartCleanup();
	}
}

FGameplayTag URoomTree::GenerateArenaType(const int32 Seed, const FRoomTreeInfo& RoomTreeInfo) const
{
	if (bOverrideArenaType)
	{
		return ArenaTypeOverride;
	}

	// Chance of generating a normal combat room, in the range [0,100]
	uint8 CombatChance = 100;

	// Chance of generating a horde room, in the range [0,100]
	uint8 HordeChance = 0;

	// Chance of generating an elite room, in the range [0,100]
	uint8 EliteChance = 0;

	if (RoomTreeInfo.Row > 1)
	{
		// Rightmost column in the current row of rooms
		const uint8 MaxColumn = RoomTreeInfo.Width - 1;

		// Number of arenas between this arena and the left edge
		const uint8 DistanceFromLeftEdge = RoomTreeInfo.Column;

		// Number of arenas between this arena and the right edge
		const uint8 DistanceFromRightEdge = MaxColumn - RoomTreeInfo.Column;

		// Number of arenas between this arena and the nearest edge
		const uint8 DistanceFromEdge = FMath::Min(DistanceFromLeftEdge, DistanceFromRightEdge);

		// Maximum number of arenas between this arena and the nearest edge
		const uint8 MaxDistanceFromEdge = MaxColumn / 2;

		// Distance to the nearest edge in the range [0.0,1.0]
		const double ScaledDistanceFromEdge = (MaxDistanceFromEdge > 0) ? (double)DistanceFromEdge / MaxDistanceFromEdge : 0.0;

		if (RoomTreeInfo.Row < RoomTreeInfo.LateRowThreshold)
		{
			// How far into the early section the row is, in the range [0.0,1.0]
			const double ScaledDepth = (RoomTreeInfo.LateRowThreshold > 1) ? (double)RoomTreeInfo.Row / (RoomTreeInfo.LateRowThreshold - 1) : 0.0;

			// In the range [0.0,1.0], where 0.0 will generate easier arena types, and 1.0 will generate harder arena types
			const double DangerRatio = ScaledDepth * ScaledDistanceFromEdge;

			EliteChance = DangerRatio * EarlyEliteMaxChance;
			HordeChance = DangerRatio * EarlyHordeMaxChance;
			CombatChance = 100 - EliteChance - HordeChance;
		}
		else
		{
			// Original range of ScaledDistanceFromEdge
			const FVector2D ScaledDistanceRange = FVector2D(0.0, 1.0);

			// New range for ScaledDistanceFromEdge
			const FVector2D RescaledDistanceRange = FVector2D(MinScaledDistance, 1.0);

			// Distance to the nearest edge in the range [MinScaledDistance,1.0]
			const double RescaledDistanceFromEdge = FMath::GetMappedRangeValueClamped(ScaledDistanceRange, RescaledDistanceRange, ScaledDistanceFromEdge);

			// In the range [0.0,1.0], where 0.0 will generate easier arena types, and 1.0 will generate harder arena types
			const double DangerRatio = RescaledDistanceFromEdge;

			EliteChance = DangerRatio * EliteMaxChance;
			HordeChance = DangerRatio * HordeMaxChance;

			// Decrease the chances of two adjacent arenas having the same type
			if ((RoomTreeInfo.Column % 2 == 0 && RoomTreeInfo.Row % 2 == 0)
				|| (RoomTreeInfo.Column % 2 == 1 && RoomTreeInfo.Row % 2 == 1))
			{
				const uint8 Change = EliteChance / 2;

				EliteChance -= Change;
				HordeChance += Change;
			}
			else
			{
				const uint8 Change = HordeChance / 2;

				EliteChance += Change;
				HordeChance -= Change;
			}

			CombatChance = 100 - EliteChance - HordeChance;
		}
	}

	// Generated number in the range [0,99]
	const int32 Gen = UIdkRandomStream::RandRange(Seed, 0, 99);

	FGameplayTag ArenaType;

	if (Gen < CombatChance)
	{
		ArenaType = TAG_Room_Arena_Combat;
	}
	else if (Gen < CombatChance + HordeChance)
	{
		ArenaType = TAG_Room_Arena_Horde;
	}
	else
	{
		ArenaType = TAG_Room_Arena_Elite;
	}

	return ArenaType;
}

FGameplayTag URoomTree::GenerateHallType(const int32 Seed, const FRoomTreeInfo& RoomTreeInfo, const FGameplayTag PrevArenaType, const bool bLeft) const
{
	if (bOverrideHallType)
	{
		return HallTypeOverride;
	}

	// Chance of generating a shop, in the range [0,100]
	uint8 ShopChance = 0;

	// Chance of generating a rest room, in the range [0,100]
	uint8 RestChance = 0;

	// Chance of generating a cursed room, in the range [0,100]
	uint8 CurseChance = 0;

	// Whether the arena this hall exits from is a horde room
	const bool bPrevHorde = PrevArenaType.MatchesTag(TAG_Room_Arena_Horde);

	// Whether the arena this hall exits from is a elite room
	const bool bPrevElite = PrevArenaType.MatchesTag(TAG_Room_Arena_Elite);

	// Column of the hall
	const uint8 Column = (bLeft) ? RoomTreeInfo.Column * 2 : (RoomTreeInfo.Column * 2) + 1;

	// Rightmost column in the current row of halls
	const uint8 MaxColumn = (RoomTreeInfo.Width * 2) - 1;

	// Number of halls between this hall and the left edge
	const uint8 DistanceFromLeftEdge = Column;

	// Number of halls between this hall and the right edge
	const uint8 DistanceFromRightEdge = MaxColumn - Column;

	// Number of halls between this hall and the nearest edge
	const uint8 DistanceFromEdge = FMath::Min(DistanceFromLeftEdge, DistanceFromRightEdge);

	// Maximum number of halls between this hall and the nearest edge
	const uint8 MaxDistanceFromEdge = MaxColumn / 2;

	// Distance to the nearest edge in the range [0.0,1.0]
	const double ScaledDistanceFromEdge = (MaxDistanceFromEdge > 0) ? (double)DistanceFromEdge / MaxDistanceFromEdge : 0.0;

	if (RoomTreeInfo.Row < RoomTreeInfo.LateRowThreshold)
	{
		// How far into the early section the row is, in the range [0.0,1.0]
		const double ScaledDepth = (RoomTreeInfo.LateRowThreshold > 1) ? (double)RoomTreeInfo.Row / (RoomTreeInfo.LateRowThreshold - 1) : 0.0;

		// In the range [0.0,1.0], where 0.0 will generate easier hall types, and 1.0 will generate harder hall types
		double DangerRatio = (ScaledDistanceFromEdge * ScaledDepth);

		if (bPrevHorde || bPrevElite)
		{
			DangerRatio *= 0.5;
		}

		ShopChance = (1.0 - DangerRatio) * EarlyShopMaxChance;
		CurseChance = DangerRatio * EarlyCurseMaxChance;
		RestChance = 100 - ShopChance - CurseChance;
	}
	else
	{
		// Original range of ScaledDistanceFromEdge
		const FVector2D ScaledDistanceRange = FVector2D(0.0, 1.0);

		// New range for ScaledDistanceFromEdge
		const FVector2D RescaledDistanceRange = FVector2D(MinScaledDistance, 1.0);

		// Distance to the nearest edge in the range [MinScaledDistance,1.0]
		const double RescaledDistanceFromEdge = FMath::GetMappedRangeValueClamped(ScaledDistanceRange, RescaledDistanceRange, ScaledDistanceFromEdge);

		// In the range [0.0,1.0], where 0.0 will generate easier hall types, and 1.0 will generate harder hall types
		double DangerRatio = RescaledDistanceFromEdge;

		if (bPrevHorde || bPrevElite)
		{
			DangerRatio *= 0.5;
		}

		ShopChance = (1.0 - DangerRatio) * ShopMaxChance;
		CurseChance = DangerRatio * CurseMaxChance;
		RestChance = 100 - ShopChance - CurseChance;
	}

	// Decrease the chances of two adjacent halls having the same type
	if ((Column % 2 == 0 && RoomTreeInfo.Row % 2 == 0)
		|| (Column % 2 == 1 && RoomTreeInfo.Row % 2 == 1))
	{
		const uint8 Change = ShopChance / 2;

		ShopChance -= Change;
		RestChance += Change;
	}
	else
	{
		const uint8 Change = RestChance / 2;

		ShopChance += Change;
		RestChance -= Change;
	}

	// Generated number in the range [0,99]
	const int32 Gen = UIdkRandomStream::RandRange(Seed, 0, 99);

	FGameplayTag HallType;

	if (Gen < ShopChance)
	{
		HallType = TAG_Room_Hall_Shop;
	}
	else if (Gen < ShopChance + CurseChance)
	{
		HallType = TAG_Room_Hall_Curse;
	}
	else
	{
		HallType = TAG_Room_Hall_Rest;
	}

	return HallType;
}

void URoomTree::FinishHallSetup(UHallNode& Hall, UArenaNode& PrevArena, const FHallSetupInfo& HallSetupInfo, FVector& InOutMin, TArray<FMapPartitionBounds*>& OutArenaHallBounds)
{
	const FVector& PrevArenaPos = PrevArena.RoomSpawnInfo->Pos;
	const uint8 ArenaLevel = PrevArena.GetRoomSpawnInfo().GetLevel();

	UArenaNode* NextArena = Hall.NextRoomNode;
	const FVector& NextArenaPos = NextArena->RoomSpawnInfo->Pos;

	if (Hall.bPortal)
	{
		const FVector HallPos = PrevArenaPos + HallSetupInfo.PortalHallOffset;

		InOutMin = FVector::Min(InOutMin, HallPos);

		Hall.RoomSpawnInfo->Pos = HallPos;

		OutArenaHallBounds.Add(new FPortalHallBounds(HallPos, HallSetupInfo.HalfHallLengthVoxel, HallSetupInfo.HalfHallWidthVoxel, HallSetupInfo.PortalHallHeightVoxel));
	}
	else
	{
		const FVector HallPos = (NextArenaPos + PrevArenaPos) / 2.0;

		Hall.RoomSpawnInfo->Pos = HallPos;

		double HallEntranceAngle = FMath::Atan2(NextArenaPos.Y - PrevArenaPos.Y, NextArenaPos.X - PrevArenaPos.X);

		if (HallEntranceAngle < 0.0)
		{
			// Convert to an equivalent positive angle
			HallEntranceAngle += UE_DOUBLE_TWO_PI;
		}

		PrevArena.RoomSpawnInfo->GapAngles.Add(HallEntranceAngle);
		Hall.RoomSpawnInfo->Angle = HallEntranceAngle;

		// Get the opposite angle
		const double HallExitAngle = (HallEntranceAngle >= UE_DOUBLE_PI) ? HallEntranceAngle - UE_DOUBLE_PI : HallEntranceAngle + UE_DOUBLE_PI;

		NextArena->RoomSpawnInfo->GapAngles.Add(HallExitAngle);

		OutArenaHallBounds.Add(new FArenaHallBounds(PrevArenaPos, NextArenaPos, HallSetupInfo.InnerRadiusSq, HallSetupInfo.OuterRadiusSq, HallSetupInfo.InnerHallWidthSq, HallSetupInfo.OuterHallWidthSq));
	}
}

UArenaNode& URoomTree::CreateArena(UIdkRandomStream& Rng, FRoomTreeInfo& RoomTreeInfo, const FGameplayTag& FixedType) const
{
	const FName NewArenaName = FName(FString::Printf(TEXT("Arena%u%u"), RoomTreeInfo.Row, RoomTreeInfo.Column));
	UArenaNode* NewArena = NewObject<UArenaNode>(GetOuter(), NewArenaName);

	const FVector Pos = FVector(RoomTreeInfo.GetXForCurrentArena(), RoomTreeInfo.GetYForCurrentArena(), ArenaBaseVoxelHeight);

	RoomTreeInfo.UpdateMinAndMax(Pos);

	FGameplayTag ArenaType = (FixedType.MatchesTag(TAG_Room)) 
		? FixedType 
		: GenerateArenaType(Rng.GenerateSeed(), RoomTreeInfo);

	NewArena->Init(*new FRoomSpawnInfo(Rng.GenerateSeed(), ArenaType, RoomTreeInfo.Row, Pos));

	if (RoomTreeInfo.ShouldCreateHall(true))
	{
		UHallNode& NewLeftHall = CreateHall(Rng, RoomTreeInfo, ArenaType, true);

		NewArena->LeftHall = &NewLeftHall;

		RoomTreeInfo.AddHall(NewLeftHall);
	}

	if (RoomTreeInfo.ShouldCreateHall(false))
	{
		UHallNode& NewRightHall = CreateHall(Rng, RoomTreeInfo, ArenaType, false);

		NewArena->RightHall = &NewRightHall;

		RoomTreeInfo.AddHall(NewRightHall);
	}

	RoomTreeInfo.AddArena(*NewArena);

	return *NewArena;
}

UHallNode& URoomTree::CreateHall(UIdkRandomStream& Rng, FRoomTreeInfo& RoomTreeInfo, const FGameplayTag& PreviousRoomType, const bool bLeft) const
{
	const FString HallSide = (bLeft) ? TEXT("Left") : TEXT("Right");
	const FName NewHallName = FName(FString::Printf(TEXT("Arena%u%u%sHall"), RoomTreeInfo.Row, RoomTreeInfo.Column, *HallSide));

	UHallNode* NewHall = NewObject<UHallNode>(GetOuter(), NewHallName);

	FGameplayTag HallType = GenerateHallType(Rng.GenerateSeed(), RoomTreeInfo, PreviousRoomType, bLeft);

	NewHall->Init(*new FRoomSpawnInfo(Rng.GenerateSeed(), HallType, RoomTreeInfo.Row + 1));

	return *NewHall;
}

URoomTree::FRoomTreeInfo::FRoomTreeInfo(const double CenterSpacingX, const double CenterSpacingY, const FVector& ArenaBoundsOffset, const uint8 LateRowThreshold, const uint8 PreBossRow)
	: CenterSpacingX(CenterSpacingX), CenterSpacingY(CenterSpacingY), ArenaBoundsOffset(ArenaBoundsOffset), LateRowThreshold(LateRowThreshold), PreBossRow(PreBossRow)
{
}

void URoomTree::FRoomTreeInfo::IncrementWidth()
{
	PrevWidth = Width++;
}

void URoomTree::FRoomTreeInfo::DecrementWidth()
{
	PrevWidth = Width--;
}

void URoomTree::FRoomTreeInfo::UpdateMinAndMax(const FVector& ArenaCenter)
{
	Min = FVector::Min(Min, ArenaCenter - ArenaBoundsOffset);
	Max = FVector::Max(Max, ArenaCenter + ArenaBoundsOffset);
}

void URoomTree::FRoomTreeInfo::AddRow(const bool bIncrementRow)
{
	Arenas.AddDefaulted();

	if (bIncrementRow)
	{
		++Row;
	}

	if (Row <= PreBossRow)
	{
		Halls.AddDefaulted();
	}
}

void URoomTree::FRoomTreeInfo::AddArena(UArenaNode& Arena)
{
	Arenas[Row].Add(&Arena);
}

void URoomTree::FRoomTreeInfo::AddHall(UHallNode& Hall)
{
	Halls[Row].Add(&Hall);
}

double URoomTree::FRoomTreeInfo::GetXForCurrentArena() const
{
	return Row * CenterSpacingX;
}

double URoomTree::FRoomTreeInfo::GetYForCurrentArena() const
{
	return ((double)Column - (((double)Width - 1.0) / 2.0)) * CenterSpacingY;
}

bool URoomTree::FRoomTreeInfo::ShouldCreateHall(const bool bLeft) const
{
	return Row < PreBossRow || (Row == PreBossRow && Column == ((bLeft) ? Width - 1 : 0));
}

void URoomTree::FRoomTreeInfo::ConnectArenaToPreviousHalls(UArenaNode& Room)
{
	if (PrevWidth < Width)
	{
		if (Column == 0)
		{
			Halls[Row - 1][0]->NextRoomNode = &Room;
		}
		else if (Column == Width - 1)
		{
			Halls[Row - 1][(2 * PrevWidth) - 1]->NextRoomNode = &Room;
		}
		else
		{
			Halls[Row - 1][(Column * 2) - 1]->NextRoomNode = &Room;
			Halls[Row - 1][Column * 2]->NextRoomNode = &Room;
		}
	}
	else
	{
		Halls[Row - 1][(Column * 2) + 1]->NextRoomNode = &Room;
		Halls[Row - 1][(Column * 2) + 2]->NextRoomNode = &Room;

		if (Column == 0)
		{
			Halls[Row - 1][(2 * PrevWidth) - 1]->NextRoomNode = &Room;
			Halls[Row - 1][(2 * PrevWidth) - 1]->bPortal = true;
		}

		if (Column == Width - 1)
		{
			Halls[Row - 1][0]->NextRoomNode = &Room;
			Halls[Row - 1][0]->bPortal = true;
		}
	}
}
