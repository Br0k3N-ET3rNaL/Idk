// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/Delegate.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "EffectPickup.generated.h"

class AIdkPlayerCharacter;
class UCapsuleComponent;
class UNiagaraComponent;
class UPrimitiveComponent;
struct FHitResult;

/** Actor which can be picked up by a player. */
UCLASS()
class AEffectPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AEffectPickup();

	/** Delegate called when the pickup is picked up. */
	FSimpleDelegate OnPickedUpDelegate;

protected:
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	/**
	 * Event called when the pickup is picked up.
	 * 
	 * @param Player	Player who picked up the pickup. 
	 */
	virtual void OnPickup(AIdkPlayerCharacter& Player) PURE_VIRTUAL(AEffectPickup::OnPickup, ;);

private:
	/** Event called when an actor overlaps the pickup's capsule. */
	UFUNCTION()
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** VFX used to visually represent the pickup. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> PickupVFX;

	/** Capsule collision component. When overlapped by the player, will call OnPickup(). */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleCollisionComp;
};
