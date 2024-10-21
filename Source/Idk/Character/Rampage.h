// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/BossEnemyCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Containers/Array.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "Rampage.generated.h"

enum class EDataValidationResult : uint8;
class AActor;
class AProjectile;
class FDataValidationContext;
class FObjectInitializer;
class UAbilityComponent;
class UAnimMontage;
class UBoxComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;
struct FBranchingPointNotifyPayload;
struct FHitResult;

/** Rampage boss enemy. */
UCLASS()
class ARampage final : public ABossEnemyCharacter
{
	GENERATED_BODY()
	
public:
	ARampage(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

private:
	//~ Begin AIdkCharacter Interface.
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	//~ End AIdkCharacter Interface

	//~ Begin AIdkEnemyCharacter Interface.
	virtual void BindAbilities() override;
	virtual void PostAbilityComponentsCreated() override;
	virtual void BasicAttack() override final;
	UE_NODISCARD virtual bool AreAbilityBonusConditionsFulfilledImpl(const int32 Index) const override;
	//~ End AIdkEnemyCharacter Interface

	/** Event called when Rampage's hand overlaps an actor. */
	UFUNCTION()
	void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Implementation for the smash ability. */
	UFUNCTION()
	void AbilitySmash();

	/** Event called when Rampage's hands hit the ground. */
	void OnSmashImpact();

	/** Implementation for the rip n' toss ability. */
	UFUNCTION()
	void AbilityRipNToss();

	/** Called after rip has completed. */
	void BeginToss();

	/** Throw the rock for the rip n' toss ability. */
	void Toss();

	/** Implementation for the enrage ability. */
	UFUNCTION()
	void AbilityEnrage();

	/** Called to activate enrage */
	void ActivateEnrage();

	/** Hitbox for Rampage's right hand. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RightHandHitbox;

	/** Hitbox for Rampage's left hand. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> LeftHandHitbox;

	/** List of actors hit by basic attacks. Used to prevent actors from being hit more than once by the same attack. */
	TArray<const AActor*> HitActors;

	/** Name of the collision profile used by the hand hitboxes. */
	const FName HandCollisionProfile = TEXT("OverlapOnlyPawn");

	/** Name of the socket to attach the right hand hitbox to. */
	const FName RightHandHitboxSocket = TEXT("FX_Trail_R_01");

	/** Name of the socket to attach the left hand hitbox to. */
	const FName LeftHandHitboxSocket = TEXT("FX_Trail_L_01");

	/** Name of the montage notify window that controls when the right hand collision is enabled and disabled. */
	const FName RightDamageWindow = TEXT("RightDamageWindow");

	/** Name of the montage notify window that controls when the left hand collision is enabled and disabled. */
	const FName LeftDamageWindow = TEXT("LeftDamageWindow");

	/** Name of the montage notify that controls when the player can basic attack again. */
	const FName CanAttackAgain = TEXT("SaveAttack");

	/** Name of the socket to attach the rock to for the rip n' toss ability. */
	const FName RockAttachSocket = TEXT("RockAttachPoint");

	/** Static mesh component of the rock used by the rip n' toss ability. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RockMeshComp;

	/** Whether Rampage is currently enraged. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	bool bEnraged = false;

	/** Abiliy info for the smash ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Smash", meta = (DisplayName = "Info"))
	FAbilityInfo SmashAbilityInfo;

	/** Animation used for the smash ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Smash", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> SmashMontage;

	/** Index of the ability component used by the smash ability. */
	int32 SmashIndex = -1;

	/** Name of the montage notify that fires when Rampage's hands hits the ground during the smash ability. */
	const FName SmashImpactNotify = TEXT("SmashImpact");

	/** Name of the socket to attach the hitbox to for the smash ability. */
	const FName SmashCollisionSocket = TEXT("SmashLocation");

	/** Ability info for the smash ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|RipNToss", meta = (DisplayName = "Info"))
	FAbilityInfo RipNTossAbilityInfo;

	/** Animation used for the rip portion of rip n' toss. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|RipNToss")
	TSoftObjectPtr<UAnimMontage> RipMontage;

	/** Animation used for the toss portion of rip n' toss. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|RipNToss")
	TSoftObjectPtr<UAnimMontage> TossMontage;

	/** Blueprint class of the projectile representing the rock for the rip n' toss ability */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|RipNToss", meta = (DisplayName = "Projectile Class"))
	TSoftClassPtr<AProjectile> RipNTossProjectileClass;

	/** Delay before throwing the boulder. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|RipNToss", meta = (Units = "s"))
	float TossDelay = 0.25;

	/** Index of the ability component used by the rip n' toss ability. */
	int32 RipNTossIndex = -1;

	/** Name of the montage notify that causes the rock to be thrown for the rip n' toss ability. */
	const FName TossReleaseNotify = TEXT("TossRelease");

	/** Ability info for the enrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Enrage", meta = (DisplayName = "Info"))
	FAbilityInfo EnrageAbilityInfo;

	/** Animation used for the enrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Enrage", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> EnrageMontage;

	/** At what health percentage to activate the enrage ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Enrage", meta = (DisplayName = "Activation Health Threshold", ClampMin = "0.0", ClampMax = "1.0"))
	double EnrageHealthThreshold = 0.5;

	/** Index of the ability component used by the enrage ability. */
	int32 EnrageIndex = -1;

};
