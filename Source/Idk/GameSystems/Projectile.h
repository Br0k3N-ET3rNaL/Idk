// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/DelegateCombinations.h>
#include <Engine/HitResult.h>
#include <Engine/TimerHandle.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "Projectile.generated.h"

class AIdkCharacter;
class UAttributeSystemComponent;
class UCapsuleComponent;
class UParticleSystemComponent;
class UPrimitiveComponent;
class UProjectileMovementComponent;
class USphereComponent;

DECLARE_DELEGATE_OneParam(FOnCollisionDelegate, UAttributeSystemComponent*);

/** A projectile with a particle system. */
UCLASS()
class AProjectile final : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	/**
	 * Initialize a non-homing projectile.
	 * 
	 * @param InitialVelocity			Starting velocity for the projectile. 
	 * @param InCollisionFilterClass	Class of characters to overlap/hit.
	 * @param OwningActor				Actor which created the projectile.
	 */
	void Init(const FVector& InitialVelocity, TSubclassOf<AIdkCharacter> InCollisionFilterClass, AActor* OwningActor);

	/**
	 * Initialize a homing projectile.
	 * 
	 * @param InitialVelocity			Starting velocity for the projectile. 
	 * @param InCollisionFilterClass	Class of characters to overlap/hit.
	 * @param OwningActor				Actor which created the projectile.
	 * @param Target					Actor to home in on.
	 */
	void InitHoming(const FVector& InitialVelocity, TSubclassOf<AIdkCharacter> InCollisionFilterClass, AActor* OwningActor, AActor* Target);

	/**
	 * Delegate called when the projectile collides with a character.
	 * 
	 * @param AttributeSystem	Attribute system component of the character that hit.
	 */
	FOnCollisionDelegate OnCollisionDelegate;

protected:
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	/** Event called when the projectile overlaps an actor. */
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Event called when the projectile hits an actor. */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Stop the particle system and then destroy the projectile. */
	UFUNCTION()
	void DeactivateProjectile();

	/** Called when the particle system has finished to destroy the projectile. */
	UFUNCTION()
	void DestroyProjectile(UParticleSystemComponent* PSystem);

	/** Enable homing for the projectile. */
	void EnableHoming();

	/** How long before the projectile is destroyed. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = "s", ClampMin = 0.1))
	float Lifetime = 10.f;

	/** How long before the projectile will start homing. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = "s", ClampMin = 0.0))
	float HomingDelay = 0.f;

	/** Amount of homing to use when enabling homing. */
	float HomingAmount = 0.f;

	/** Collision component for the tip of the projectile. Used for hits. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComp;

	/** Collision component for the body of the projectile. Used for overlaps. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleComp;

	/** Projectile movement component. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> MoveComp;

	/** Particle system component. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> ParticleSystemComp;

	/** Class of characters to overlap/hit. */
	UPROPERTY(VisibleInstanceOnly)
	TSubclassOf<AIdkCharacter> CollisionFilterClass;

	/** Timer for the lifetime of the projectile. */
	FTimerHandle LifetimeTimer;

	/** Timer for the delay before enabling homing. */
	FTimerHandle HomingDelayTimer;

};
