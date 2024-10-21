// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/MapManager.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/IdkRandomStream.h"
#include "Idk/GameSystems/IdkVoxelGenerator.h"
#include "Idk/GameSystems/RoomManagers/ArenaMeshManager.h"
#include "Idk/GameSystems/RoomManagers/ArenaRoomManager.h"
#include "Idk/GameSystems/RoomManagers/BossRoomManager.h"
#include "Idk/GameSystems/RoomManagers/CombatRoomManager.h"
#include "Idk/GameSystems/RoomManagers/CurseRoomManager.h"
#include "Idk/GameSystems/RoomManagers/EliteRoomManager.h"
#include "Idk/GameSystems/RoomManagers/HallMeshManager.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include "Idk/GameSystems/RoomManagers/HordeRoomManager.h"
#include "Idk/GameSystems/RoomManagers/RestRoomManager.h"
#include "Idk/GameSystems/RoomManagers/RoomManagerClasses.h"
#include "Idk/GameSystems/RoomManagers/ShopRoomManager.h"
#include "Idk/GameSystems/RoomManagers/SpawnRoomManager.h"
#include "Idk/GameSystems/RoomSpawnInfo.h"
#include "Idk/GameSystems/RoomTree.h"
#include "Idk/GameSystems/RoomTypes.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/MidRunSaveGame.h"
#include "Idk/UI/MapInitInfo.h"
#include "Idk/UI/MapWidget.h"
#include <Camera/CameraTypes.h>
#include <Components/SceneCaptureComponent.h>
#include <Components/SceneCaptureComponent2D.h>
#include <Components/StaticMeshComponent.h>
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Engine/CollisionProfile.h>
#include <Engine/EngineTypes.h>
#include <Engine/StaticMesh.h>
#include <Engine/TextureRenderTarget2D.h>
#include <Engine/TimerHandle.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <GameFramework/PlayerController.h>
#include <GameplayTagContainer.h>
#include <GenericPlatform/GenericPlatformMath.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/BoxSphereBounds.h>
#include <Math/MathFwd.h>
#include <Math/TransformCalculus2D.h>
#include <Math/UnrealMathUtility.h>
#include <Math/UnrealPlatformMathSSE.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <NavigationSystem.h>
#include <NavMesh/NavMeshBoundsVolume.h>
#include <Templates/Casts.h>
#include <Templates/PimplPtr.h>
#include <Templates/SharedPointer.h>
#include <Templates/SubclassOf.h>
#include <UObject/UObjectGlobals.h>
#include <VoxelIntBox.h>
#include <VoxelWorld.h>
#include <Widgets/Layout/Anchors.h>

AMapManager::AMapManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Rng = CreateDefaultSubobject<UIdkRandomStream>(TEXT("Rng"));

	RoomTree = CreateDefaultSubobject<URoomTree>(TEXT("RoomTree"));
}

#if WITH_EDITOR
EDataValidationResult AMapManager::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (ArenaMeshManagerClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Arena mesh manager class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (HallMeshManagerClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Hall mesh manager class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (VoxelMaterial.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Voxel material is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (RoomManagerClasses.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Room manager classes is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!SpawnableEnemyClasses)
	{
		Context.AddError(FText::FromString(TEXT("Spawnable enemy classes is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!EliteEncounters)
	{
		Context.AddError(FText::FromString(TEXT("Elite encounters is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!BossEncounters)
	{
		Context.AddError(FText::FromString(TEXT("Boss encounters is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!SpawnBudgetCurve)
	{
		Context.AddError(FText::FromString(TEXT("Spawn budget curve is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (PlaneMesh.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Plane mesh is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AMapManager::Destroyed()
{
	if (VoxelWorld)
	{
		VoxelWorld->Destroy();
	}

	if (RoomTree)
	{
		RoomTree->StartCleanup();
		RoomTree = nullptr;
	}

	Super::Destroyed();
}

void AMapManager::BeginPlay()
{
	Super::BeginPlay();

	check(!ArenaMeshManagerClass.IsNull());
	check(!HallMeshManagerClass.IsNull());
	check(!VoxelMaterial.IsNull());
	check(!RoomManagerClasses.IsNull());
	check(SpawnableEnemyClasses);
	check(EliteEncounters);
	check(BossEncounters);

	double HallWidth = 0.0;

	InitializeMeshManagers(HallWidth);

	GameInstance = CastChecked<UIdkGameInstance>(GetGameInstance());

	if (Save)
	{
		Rng->InitFromSavedSeed(Save->GetSeed());
	}
	else
	{
		Rng->Init(true);

		GameInstance->SaveMapSeed(Rng->GetInitialSeed());
	}

	VoxelWorld = GetWorld()->SpawnActorDeferred<AVoxelWorld>(AVoxelWorld::StaticClass(), FTransform());

	// Initialize the voxel world
	VoxelWorld->bCreateGlobalPool = false;
	VoxelWorld->NumberOfThreads = 1;
	VoxelWorld->VoxelSize = VoxelSize;
	VoxelWorld->bEnableNavmesh = true;
	VoxelWorld->bUseCameraIfNoInvokersFound = true;
	VoxelWorld->MaxLOD = 0;
	VoxelWorld->bConstantLOD = true;
	VoxelWorld->VoxelMaterial = VoxelMaterial.LoadSynchronous();

	VoxelWorld->SetGeneratorClass(UIdkVoxelGenerator::StaticClass());

	UIdkVoxelGenerator* VoxelGenerator = NewObject<UIdkVoxelGenerator>(VoxelWorld);

	TArray<UArenaNode*> Arenas;
	TArray<FMapPartitionBounds*> MapPartitionBounds;

	RoomTree->Init(Rng->GenerateSeed(), VoxelSize, Arenas, MapPartitionBounds);

	InitializeVoxelWorldSize();

	VoxelGenerator->Map = MakeShareable(new FMapPartition);
	VoxelGenerator->Map->Init(MapPartitionBounds);

	VoxelWorld->SetGeneratorObject(VoxelGenerator);

	// Map bounds in voxels
	const FBox VoxelBounds = RoomTree->GetBounds();

	// Map bounds in cm
	const FBox Bounds = FBox(VoxelBounds.Min * VoxelSize, VoxelBounds.Max * VoxelSize);

	// Get the navmesh volume
	NavMeshVolume = CastChecked<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));

	// Initialize the navmesh volume
	NavMeshVolume->SetActorScale3D(Bounds.GetExtent() / NavMeshVolume->Brush->Bounds.GetBox().GetExtent());
	NavMeshVolume->SetActorLocation(Bounds.GetCenter());

	UGameplayStatics::FinishSpawningActor(VoxelWorld, {});

	InitializeMap(Bounds, HallWidth);

	SpawnPlanes(Bounds);

	VoxelWorld->CreateWorld();

	// Update navmesh
	UNavigationSystemV1::GetCurrent(GetWorld())->Build();

	const FVector ArenaExtents = RoomTree->GetArenaExtents();
	const FVector HallExtents = RoomTree->GetHallExtents();

	// Spawn all the room managers
	for (UArenaNode* Arena : Arenas)
	{
		check(Arena);

		SpawnArenaRoomManager(*Arena, ArenaExtents, HallExtents);
	}

	// Link all the halls and arenas
	for (UArenaNode* Arena : Arenas)
	{
		const FRoomSpawnInfo& ArenaSpawnInfo = Arena->GetRoomSpawnInfo();
		const FGameplayTag ArenaType = ArenaSpawnInfo.GetType();

		if (ArenaType.IsValid())
		{
			AArenaRoomManager& RoomManager = Arena->GetRoomManager();

			if (UHallNode* LeftHall = Arena->GetLeftHall())
			{
				LinkHallToArenas(*LeftHall, RoomManager, true);
			}

			if (UHallNode* RightHall = Arena->GetRightHall())
			{
				LinkHallToArenas(*RightHall, RoomManager, false);
			}
		}

		Arena->ReleaseRoomSpawnInfo();
	}

	// Enable the spawn room manager
	Arenas[0]->GetRoomManager().Enable();

	VoxelWorld->OnWorldLoaded.AddDynamic(this, &AMapManager::SpawnPlayer);
}

void AMapManager::SetPlayerClass(TSubclassOf<AIdkPlayerCharacter> InPlayerClass)
{
	PlayerClass = InPlayerClass;
}

void AMapManager::LoadFromSave(const UMidRunSaveGame& InSave)
{
	Save = &InSave;
}

void AMapManager::UpdatePlayerProgress(const EPathTaken PathTaken)
{
	if (!bSimulatingPlayerProgress)
	{
		GameInstance->UpdatePlayerProgress(PathTaken);
	}
}

void AMapManager::InitializeMeshManagers(double& OutHallWidth)
{
	AArenaMeshManager::InitStatics(ArenaMeshManagerClass.LoadSynchronous(), RoomTree->GetRadius(), RoomTree->GetHallWidth());

	const double ValidRadius = AArenaMeshManager::GetValidRadius();
	const double ValidAngle = AArenaMeshManager::GetValidAngle(RoomTree->GetMinArenaSpacing(), RoomTree->GetHallLength());
	const double DoorWidth = AArenaMeshManager::GetGapWidth();
	const double HallGapEdgeWidth = AArenaMeshManager::GetPathGapEdgeWidth();

	const double MinHallLength = RoomTree->GetHallLength() + (2.0 * HallGapEdgeWidth);

	AHallMeshManager::InitStatics(HallMeshManagerClass.LoadSynchronous(), MinHallLength, DoorWidth);

	const double OuterHallLength = AHallMeshManager::GetCalculatedLength();
	const double InnerHallLength = OuterHallLength - (2.0 * HallGapEdgeWidth);
	const double ValidHallWidth = AHallMeshManager::GetCalculatedWidth();

	RoomTree->SetCalculatedParameters(ValidRadius, OuterHallLength, InnerHallLength, ValidHallWidth, ValidAngle);

	OutHallWidth = ValidHallWidth;
}

void AMapManager::InitializeVoxelWorldSize()
{
	const FBox VoxelBounds = RoomTree->GetBounds();

	const int32 MinX = FGenericPlatformMath::CeilToInt(VoxelBounds.Min.X);
	const int32 MinY = FGenericPlatformMath::CeilToInt(VoxelBounds.Min.Y);
	const int32 MaxX = FGenericPlatformMath::CeilToInt(VoxelBounds.Max.X);
	const int32 MaxY = FGenericPlatformMath::CeilToInt(VoxelBounds.Max.Y);

	VoxelWorld->SetWorldSize(FMath::Max(2 * (MaxX - MinX), MaxY - MinY));
	VoxelWorld->bUseCustomWorldBounds = true;
	VoxelWorld->CustomWorldBounds = FVoxelIntBox(FIntVector(MinX, MinY, -32), FIntVector(MaxX, MaxY, 32));
}

void AMapManager::InitializeMap(const FBox& Bounds, const double HallWidth)
{
	const int32 MaxMapSize = 2048;

	// Size of the world in cm
	const FVector WorldSize = Bounds.GetSize();
	int32 MapWidth = FMath::CeilToInt32(WorldSize.X);
	int32 MapLength = FMath::CeilToInt32(WorldSize.Y);

	// Scale the shorter side to maintain the aspect ratio
	if (MapWidth > MapLength)
	{
		MapLength *= (double)MaxMapSize / MapWidth;
		MapWidth = MaxMapSize;
	}
	else
	{
		MapWidth *= (double)MaxMapSize / MapLength;
		MapLength = MaxMapSize;
	}

	const FBox2D Bounds2D = FBox2D(FVector2D(Bounds.Min), FVector2D(Bounds.Max));
	const FBox2D MapBounds = FBox2D(FVector2D(0.0, 0.0), FVector2D(MapWidth, MapLength));

	const FScale2d MapRelativeScale = FScale2d(MapBounds.GetExtent() / Bounds2D.GetExtent());
	const FVector2D MapOrigin = MapBounds.Min + MapRelativeScale.TransformVector(-Bounds2D.Min);

	const FVector2D MapAnchor = MapOrigin / MapBounds.GetSize();
	const FAnchors MapAnchors = FAnchors(MapAnchor.X, MapAnchor.Y, MapAnchor.X, MapAnchor.Y);

	FVector BoundsCenter = Bounds.GetCenter();
	BoundsCenter.Z = Bounds.Max.Z;

	UTextureRenderTarget2D* MapImage = NewObject<UTextureRenderTarget2D>(this);
	MapImage->ResizeTarget(MapWidth, MapLength);

	MapSceneCapture = NewObject<USceneCaptureComponent2D>(this);

	// Initialize the scene capture for the map
	MapSceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MapSceneCapture->OrthoWidth = WorldSize.X;
	MapSceneCapture->TextureTarget = MapImage;
	MapSceneCapture->bCaptureEveryFrame = false;
	MapSceneCapture->bCaptureOnMovement = false;
	MapSceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	MapSceneCapture->ShowOnlyActors.Add(VoxelWorld);
	MapSceneCapture->bEnableClipPlane = true;
	MapSceneCapture->ClipPlaneBase = BoundsCenter;
	MapSceneCapture->ClipPlaneNormal = GetActorUpVector();

	MapSceneCapture->SetWorldLocation(BoundsCenter);

	// Rotate the capture 90 degrees
	MapSceneCapture->SetRelativeRotation(FQuat(FVector::DownVector, UE_DOUBLE_HALF_PI) * FVector::DownVector.ToOrientationQuat());

	MapSceneCapture->RegisterComponent();

	MapInitInfo = MakePimpl<FMapInitInfo>(*MapImage, MapRelativeScale, MapAnchors, HallWidth);
}

void AMapManager::SpawnPlanes(const FBox& Bounds)
{
	TopPlane = NewObject<UStaticMeshComponent>(this);
	TopPlane->RegisterComponent();
	TopPlane->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	BottomPlane = NewObject<UStaticMeshComponent>(this);
	BottomPlane->RegisterComponent();
	BottomPlane->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	UStaticMesh* Plane = PlaneMesh.LoadSynchronous();

	TopPlane->SetStaticMesh(Plane);
	BottomPlane->SetStaticMesh(Plane);

	const FVector2D PlanePos2D = FVector2D(Bounds.GetCenter());

	TopPlane->SetWorldScale3D(Bounds.GetExtent() / Plane->GetBounds().BoxExtent);
	TopPlane->SetWorldLocation(FVector(PlanePos2D, -1.0));
	BottomPlane->SetWorldScale3D(Bounds.GetExtent() / Plane->GetBounds().BoxExtent);
	BottomPlane->SetWorldLocation(FVector(PlanePos2D, -21.0));

	TopPlane->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	BottomPlane->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
}

void AMapManager::SpawnArenaRoomManager(UArenaNode& Arena, const FVector& ArenaExtents, const FVector& HallExtents)
{
	const FRoomSpawnInfo& ArenaSpawnInfo = Arena.GetRoomSpawnInfo();
	const FGameplayTag ArenaType = ArenaSpawnInfo.GetType();

	// Position of the arena in cm
	const FVector Pos = ArenaSpawnInfo.GetPos() * VoxelSize;

	MapInitInfo->AddArena(FMapRoomInfo(ArenaType, FVector2D(Pos)));

	AArenaRoomManager* ArenaRoomManager = nullptr;

	if (ArenaType.MatchesTagExact(TAG_Room_Arena_Combat))
	{
		ACombatRoomManager* CombatRoomManager =
			GetWorld()->SpawnActor<ACombatRoomManager>(RoomManagerClasses.LoadSynchronous()->Combat);

		CombatRoomManager->Init(ArenaSpawnInfo, ArenaExtents);
		CombatRoomManager->SetSpawnerInfo(*SpawnableEnemyClasses, *SpawnBudgetCurve);

		ArenaRoomManager = CombatRoomManager;
	}
	else if (ArenaType.MatchesTagExact(TAG_Room_Arena_Horde))
	{
		AHordeRoomManager* HordeRoomManager =
			GetWorld()->SpawnActor<AHordeRoomManager>(RoomManagerClasses.LoadSynchronous()->Horde);

		HordeRoomManager->Init(ArenaSpawnInfo, ArenaExtents);
		HordeRoomManager->SetSpawnerInfo(*SpawnableEnemyClasses, *SpawnBudgetCurve);

		ArenaRoomManager = HordeRoomManager;
	}
	else if (ArenaType.MatchesTagExact(TAG_Room_Arena_Elite))
	{
		AEliteRoomManager* EliteRoomManager =
			GetWorld()->SpawnActor<AEliteRoomManager>(RoomManagerClasses.LoadSynchronous()->Elite);

		EliteRoomManager->Init(ArenaSpawnInfo, ArenaExtents);
		EliteRoomManager->SetEncounters(*EliteEncounters);

		ArenaRoomManager = EliteRoomManager;
	}
	else if (ArenaType.MatchesTagExact(TAG_Room_Spawn))
	{
		ASpawnRoomManager* SpawnArenaRoomManager =
			GetWorld()->SpawnActor<ASpawnRoomManager>(RoomManagerClasses.LoadSynchronous()->Spawn);

		SpawnArenaRoomManager->Init(ArenaSpawnInfo, ArenaExtents);

		ArenaRoomManager = SpawnArenaRoomManager;
	}
	else if (ArenaType.MatchesTagExact(TAG_Room_Boss))
	{
		ABossRoomManager* BossRoomManager =
			GetWorld()->SpawnActor<ABossRoomManager>(RoomManagerClasses.LoadSynchronous()->Boss);

		BossRoomManager->Init(ArenaSpawnInfo, ArenaExtents);
		BossRoomManager->SetEncounters(*BossEncounters);

		ArenaRoomManager = BossRoomManager;
	}

	check(ArenaRoomManager);

	ArenaRoomManager->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ArenaRoomManager->SetActorLocation(Pos);

	if (!ArenaType.MatchesTagExact(TAG_Room_Spawn))
	{
		// Will automatically update player progress when the room is completed
		ArenaRoomManager->RoomCompletedDelegate.AddUObject(this, &AMapManager::UpdatePlayerProgress, EPathTaken::Arena);
	}

	Arena.SetRoomManager(*ArenaRoomManager);

	if (UHallNode* LeftHall = Arena.GetLeftHall())
	{
		// Spawn left hall manager
		SpawnHallRoomManager(*LeftHall, *ArenaRoomManager, HallExtents, true);
	}

	if (UHallNode* RightHall = Arena.GetRightHall())
	{
		// Spawn right hall manager
		SpawnHallRoomManager(*RightHall, *ArenaRoomManager, HallExtents, false);
	}
	
}

void AMapManager::SpawnHallRoomManager(UHallNode& Hall, AArenaRoomManager& PrevArenaRoomManager, const FVector& HallExtents, const bool bLeft)
{
	const FRoomSpawnInfo& HallSpawnInfo = Hall.GetRoomSpawnInfo();
	const FGameplayTag HallType = HallSpawnInfo.GetType();

	const FVector HallPos = HallSpawnInfo.GetPos() * VoxelSize;

	if (!Hall.IsPortal())
	{
		MapInitInfo->AddArena(FMapRoomInfo(HallType, FVector2D(HallPos)));
	}

	AHallRoomManager* HallRoomManager = nullptr;

	if (HallType.MatchesTag(TAG_Room_Hall_Shop))
	{
		HallRoomManager = GetWorld()->SpawnActor<AShopRoomManager>(RoomManagerClasses.LoadSynchronous()->Shop);
	}
	else if (HallType.MatchesTag(TAG_Room_Hall_Rest))
	{
		HallRoomManager = GetWorld()->SpawnActor<ARestRoomManager>(RoomManagerClasses.LoadSynchronous()->Rest);
	}
	else if (HallType.MatchesTag(TAG_Room_Hall_Curse))
	{
		HallRoomManager = GetWorld()->SpawnActor<ACurseRoomManager>(RoomManagerClasses.LoadSynchronous()->Curse);
	}

	check(HallRoomManager);

	HallRoomManager->GetRootComponent()->AttachToComponent(
		RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	HallRoomManager->SetActorLocation(HallPos);

	HallRoomManager->Init(HallSpawnInfo, HallExtents, Hall.IsPortal());

	// Will automatically enable the hall when the previous room is completed
	PrevArenaRoomManager.RoomCompletedDelegate.AddUObject(HallRoomManager, &AHallRoomManager::Enable);

	// Will automatically disable the previous room when the hall is activated
	HallRoomManager->RoomActivatedDelegate.AddUObject(&PrevArenaRoomManager, &AArenaRoomManager::Disable);

	// Will automatically update player progress when the hall is completed
	HallRoomManager->RoomCompletedDelegate.AddUObject(this, &AMapManager::UpdatePlayerProgress, (bLeft) ? EPathTaken::LeftHall : EPathTaken::RightHall);

	Hall.SetRoomManager(*HallRoomManager);
}

void AMapManager::LinkHallToArenas(UHallNode& Hall, AArenaRoomManager& PrevArenaRoomManager, const bool bLeft)
{
	AHallRoomManager& HallRoomManager = Hall.GetRoomManager();
	AArenaRoomManager& NextArenaRoomManager = Hall.GetNextRoomManager();

	// Will automatically enable the next room when the hall is completed
	HallRoomManager.RoomCompletedDelegate.AddUObject(&NextArenaRoomManager, &AArenaRoomManager::Enable);

	// Will automatically disable the hall when the next room is activated
	NextArenaRoomManager.RoomActivatedDelegate.AddUObject(&HallRoomManager, &AHallRoomManager::Disable);

	if (Hall.IsPortal())
	{
		PrevArenaRoomManager.EnableSpawnPortal(bLeft);
		PrevArenaRoomManager.GetPortalDestDelegate.BindUObject(&HallRoomManager, &AHallRoomManager::GetPortalDestination);
		HallRoomManager.GetPortalDestDelegate.BindUObject(&NextArenaRoomManager, &AArenaRoomManager::GetPortalDestination, bLeft);

		const FTransform Src = PrevArenaRoomManager.GetPortalSource();
		const FTransform Dest = NextArenaRoomManager.GetPortalDestination(bLeft);
		const FGameplayTag HallType = Hall.GetRoomSpawnInfo().GetType();

		MapInitInfo->AddPortal(FMapPortalInfo(
			HallType, bLeft, FVector2D(Src.GetLocation()), FVector2D(Dest.GetLocation())));
	}

	Hall.ReleaseRoomSpawnInfo();
}

void AMapManager::SpawnPlayer()
{
	VoxelWorld->OnWorldLoaded.RemoveDynamic(this, &AMapManager::SpawnPlayer);

	check(!PlayerClass.IsNull());

	FVector SpawnLocation;

	if (Save)
	{
		bSimulatingPlayerProgress = true;

		RoomTree->SimulatePlayerProgress(Save->GetPlayerProgress(), SpawnLocation);

		bSimulatingPlayerProgress = false;
	}
	else
	{
		SpawnLocation = GetActorLocation();
		SpawnLocation.Z += VoxelSize;
	}

	SpawnLocation.Z += CastChecked<AIdkPlayerCharacter>(
		PlayerClass.LoadSynchronous()->GetDefaultObject())->GetSpawnZOffset();
	
	const FRotator SpawnRotation = FRotator(0.0);
	AIdkPlayerCharacter* Player = 
		GetWorld()->SpawnActor<AIdkPlayerCharacter>(PlayerClass.LoadSynchronous(), SpawnLocation, SpawnRotation);

	check(Player);
	check(VoxelWorld->IsLoaded());
	check(VoxelWorld->IsCreated());

	FTimerHandle Timer;

	// Delay to give time for the terrain to finish loading
	GetWorldTimerManager().SetTimer(Timer, [&, this, Player] () 
		{
			MapSceneCapture->CaptureScene();

			const FMapInitInfo* MapInitInfoTemp = MapInitInfo.Get();
			const int32 PlayerSeed = Rng->GenerateSeed();

			auto InitPlayerLambda = [=, this]()
				{
					Player->InitMapWidget(*MapInitInfoTemp);
					Player->InitSeed(PlayerSeed);

					OnPlayerSpawnedDelegate.Execute();

					Player->EnableInput(CastChecked<APlayerController>(Player->GetController()));
				};

			// Check if the player has been possessed yet
			if (Player->GetController())
			{
				InitPlayerLambda();
			}
			else
			{
				Player->OnPossessedDelegate.BindLambda(InitPlayerLambda);
			}

			GameInstance->SetPlayer(*Player);
		}
	, 1.f, false);
}
