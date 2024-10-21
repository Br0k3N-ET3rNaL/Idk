// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Portal.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/IdkGameInstance.h"
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SceneCaptureComponent2D.h>
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/Engine.h>
#include <Engine/HitResult.h>
#include <Engine/TextureRenderTarget2D.h>
#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

const FName APortal::SceneCaptureMaterialParam = TEXT("SceneCapture");

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bAllowTickBeforeBeginPlay = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	PortalMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMeshComp"));
	PortalCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalCollisionComp"));
	DestComp = CreateDefaultSubobject<USceneComponent>(TEXT("DestComp"));
	SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp"));

	PortalMeshComp->SetupAttachment(RootComponent);
	PortalCollisionComp->SetupAttachment(PortalMeshComp);
	DestComp->SetupAttachment(RootComponent);
	SceneCaptureComp->SetupAttachment(DestComp);

	RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("PortalRenderTarget"));

	PortalMeshComp->SetRelativeRotation(FRotator(0.0, 90.0, 90.0));

	PortalCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBoxBeginOverlap);
	PortalCollisionComp->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SceneCaptureComp->HideActorComponents(this);
	SceneCaptureComp->bEnableClipPlane = true;
	
}

#if WITH_EDITOR
EDataValidationResult APortal::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PortalMaterial.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Portal material is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void APortal::BeginPlay()
{
	Super::BeginPlay();

	UIdkGameInstance* GameInstance = UIdkGameInstance::GetGameInstance(GetWorld());

	check(GameInstance);

	if (GameInstance->GetPlayer())
	{
		Player = GameInstance->GetPlayer();
	}
	else
	{
		GameInstance->OnPlayerSetDelegate.AddLambda([&](AIdkPlayerCharacter* InPlayer)
			{
				Player = InPlayer;
			});
	}
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player)
	{
		// Offset from the entrance portal to the player's camera
		const FVector Offset = Player->GetCameraLocation() - Source.GetLocation();

		// Offset from the exit portal to the scene capture component
		const FVector OffsetUnrot = Source.GetRotation().UnrotateVector(Offset);

		SceneCaptureComp->SetRelativeLocation(OffsetUnrot);

		// Difference in rotation between the entrance portal and the player's camera
		const FQuat RelativeRotation = Source.GetRotation().Inverse() * Player->GetCameraRotation();

		SceneCaptureComp->SetRelativeRotation(RelativeRotation);
	}
}

void APortal::Init(const FTransform& Src, const FTransform& Dest)
{
	const FBox PortalMeshBounds = PortalMeshComp->GetStaticMesh()->GetBoundingBox();
	const FVector Offset = FVector(0.0, 0.0, PortalMeshBounds.GetExtent().Y * PortalMeshComp->GetRelativeScale3D().Y);

	Source = Src;
	Source.AddToTranslation(Offset);

	Destination = Dest;
	Destination.AddToTranslation(Offset);

	DestComp->SetWorldTransform(Destination);

	PortalMeshComp->SetWorldLocation(Source.GetLocation());
	PortalMeshComp->AddWorldRotation(Source.GetRotation());

	SceneCaptureComp->ClipPlaneBase = Destination.GetLocation();
	SceneCaptureComp->ClipPlaneNormal = Destination.GetRotation().GetForwardVector();

	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	RenderTarget->ResizeTarget(ViewportSize.X, ViewportSize.Y);
	SceneCaptureComp->TextureTarget = RenderTarget;

	check(!PortalMaterial.IsNull());

	UMaterialInstanceDynamic* PortalMaterialInst =
		UMaterialInstanceDynamic::Create(PortalMaterial.LoadSynchronous(), this);

	PortalMaterialInst->SetTextureParameterValue(SceneCaptureMaterialParam, RenderTarget);

	PortalMeshComp->SetMaterial(0, PortalMaterialInst);
}

void APortal::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Player)
	{
		OnPortalEnteredDelegate.ExecuteIfBound();

		AController* PlayerController = Player->GetController();

		// Rotation of the player relative to the entrance portal
		const FRotator PlayerRelativeRotation = Source.GetRotation().Rotator() - PlayerController->GetControlRotation();

		// Rotation of the player relative to the exit portal
		const FRotator PlayerRotation = Destination.GetRotation().Rotator() + PlayerRelativeRotation/* - FRotator(0.0, 180.0, 0.0)*/;

		Player->TeleportTo(Destination.GetLocation(), PlayerRotation);

		Player->GetController()->SetControlRotation(PlayerRotation);
	}
}

