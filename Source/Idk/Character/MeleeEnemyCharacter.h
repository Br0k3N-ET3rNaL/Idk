// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkEnemyCharacter.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "MeleeEnemyCharacter.generated.h"

enum class EDataValidationResult : uint8;
class AActor;
class FDataValidationContext;
class FObjectInitializer;
class UAbilityComponent;
class UBoxComponent;
class UPrimitiveComponent;
struct FBranchingPointNotifyPayload;
struct FHitResult;
struct FPropertyChangedEvent;

/** Base class for enemies wielding two swords. */
UCLASS()
class AMeleeEnemyCharacter : public AIdkEnemyCharacter
{
	GENERATED_BODY()

public:
	AMeleeEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin AIdkCharacter Interface.
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	//~ End AIdkCharacterInterface

	//~ Begin AIdkEnemyCharacter Interface.
	virtual void BasicAttack() override;
	//~ End AIdkEnemyCharacter Interface

	/** Event called when sword hitbox overlaps an actor. Handles applying basic attack effects. */
	UFUNCTION()
	void OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Gets the names of all sockets on the character's skeletal mesh. */
	UFUNCTION()
	TArray<FName> GetSocketNames() const;

	/** Hitbox for the enemy's right sword. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RightSwordHitbox;

	/** Hitbox for the enemy's left sword. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> LeftSwordHitbox;

	/** Name of the socket to attach the right sword hitbox to. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (GetOptions = "GetSocketNames"))
	FName RightSwordHitboxSocket = NAME_None;

	/** Name of the socket to attach the left sword hitbox to. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (GetOptions = "GetSocketNames"))
	FName LeftSwordHitboxSocket = NAME_None;

	/** List of actors hit by basic attacks. Used to prevent actors from being hit more than once by the same attack. */
	TArray<const AActor*> HitActors;

	/** Name of the montage notify window that controls when the right sword collision is enabled and disabled. */
	const FName RightDamageWindow = FName(TEXT("RightDamageWindow"));

	/** Name of the montage notify window that controls when the left sword collision is enabled and disabled. */
	const FName LeftDamageWindow = FName(TEXT("LeftDamageWindow"));

	/** Name of the montage notify that controls when the player can basic attack again. */
	const FName CanAttackAgain = FName(TEXT("SaveAttack"));
};
