// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "Portal.generated.h"

enum class EDataValidationResult : uint8;
class AIdkPlayerCharacter;
class FDataValidationContext;
class UBoxComponent;
class UMaterialInterface;
class UPrimitiveComponent;
class USceneComponent;
class USceneCaptureComponent2D;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
struct FHitResult;

/** Allows the player to teleport between two locations. */
UCLASS()
class APortal final : public AActor
{
	GENERATED_BODY()
	
public:	
	APortal();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface
	// 
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface

	/**
	 * Initialize the portal.
	 * 
	 * @param Source	Transform for the entrance portal. 
	 * @param Dest		Transform for the exit portal.
	 */
	void Init(const FTransform& Source, const FTransform& Dest);

	/** Delegate called when the player enters the entrance portal. */
	FSimpleDelegate OnPortalEnteredDelegate;

private:
	/** Event called when an actor overlaps the entrance portal hitbox. */
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Material used for the front of the entrance portal. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UMaterialInterface> PortalMaterial;

	/** Render target used by the scene capture component. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	/** Static mesh component representing the entrance portal. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PortalMeshComp;

	/** Hitbox for the entrance portal. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> PortalCollisionComp;

	/** Scene capture component used to capture the scene at the exit portal. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComp;

	/** Transform of the entrance portal. */
	UPROPERTY(VisibleAnywhere)
	FTransform Source;

	/** Transform of the exit portal. */
	UPROPERTY(VisibleAnywhere)
	FTransform Destination;

	/** Scene component at the location of the exit portal. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DestComp;

	/** Current player character. */
	TObjectPtr<AIdkPlayerCharacter> Player;

	/** Name of the material parameter used for the scene capture. */
	static const FName SceneCaptureMaterialParam;

};
