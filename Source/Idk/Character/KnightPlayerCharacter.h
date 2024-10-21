// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "KnightPlayerCharacter.generated.h"

enum class EDataValidationResult : uint8;
class AActor;
class FDataValidationContext;
class FObjectInitializer;
class UAnimMontage;
class UAbilityComponent;
class UAudioComponent;
class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UPrimitiveComponent;
struct FBranchingPointNotifyPayload;
struct FHitResult;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AKnightPlayerCharacter final : public AIdkPlayerCharacter
{
	GENERATED_BODY()
	
public:
	AKnightPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Start UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin ACharacter Interface.
	virtual void Landed(const FHitResult& Hit) override;
	//~ End ACharacter Interface

private:
	//~ Begin AIdkCharacter Interface.
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override final;
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;
	//~ End AIdkCharacter Interface

	//~ Begin AIdkPlayerCharacter Interface.
	virtual void BasicAttackImpl() override final;
	//~ End AIdkPlayerCharacter Interface

	/** Event called when sword hitbox overlaps an actor. Handles applying basic attack effects. */
	UFUNCTION()
	void OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Enables collision for the sword hitbox. */
	void EnableSwordCollision();

	/** Disables collision for the sword hitbox. */
	void DisableSwordCollision();

	/** Implementation for the leap ability. */
	void AbilityLeap();

	/** Implementation for the whirlwind ability. */
	void AbilityWhirlwind();


	void OnWhirlwindEnd();

	/** Implementation for the deflect ability. */
	void AbilityDeflect();

	/** Component used to play a sound effect when basic attacking. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> SwordSwingAudioComp;

	/** Hitbox for the knight's sword. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> SwordHitbox;

	/** Animation used for the leap ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Leap", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> LeapMontage;

	/** The amount of time it takes for the leap animation to land. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Abilities|Leap", meta = (DisplayName = "Air Time"))
	double LeapAirTime = 0.88;

	/** Ability info for the leap ability. @see FAbilityInfo */
	UPROPERTY(EditAnywhere, Category = "Abilities|Leap", meta = (DisplayName = "Info", EditCondition = "!bBegunPlay", HideEditConditionToggle))
	FAbilityInfo LeapAbilityInfo;

	/** Index of the ability component used by the leap ability. */
	int32 LeapIndex = -1;

	/** Ability info for the whirlwind ability. @see FAbilityInfo */
	UPROPERTY(EditAnywhere, Category = "Abilities|Whirlwind", meta = (DisplayName = "Info", EditCondition = "!bBegunPlay", HideEditConditionToggle))
	FAbilityInfo WhirlwindAbilityInfo;

	/** Animation used for the whirlwind ability */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Whirlwind", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> WhirlwindMontage;

	/** VFX used for the whirlwind ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Whirlwind", meta = (DisplayName = "VFX"))
	TSoftObjectPtr<UParticleSystem> WhirlwindVFX;

	/** Delay between activating whirlwind and applying damage to targets. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Whirlwind", meta = (DisplayName = "Activation Delay"))
	double WhirlwindActivationDelay = 0.2;

	/** Index of the ability component used by the whirlwind ability. */
	int32 WhirlwindIndex = -1;

	/** Name of the mesh socket to attach whirlwind's ability component and VFX to. */
	static const FName WhirlwindSocket;

	/** Timer that applies whirlwind's effects to target at regular intervals for the duration. */
	FTimerHandle WhirlwindApplyEffectTimer;

	/** Timer that controls the active duration of whirlwind. */
	FTimerHandle WhirlwindDurationTimer;

	/** Ability info for the deflect ability. @see FAbilityInfo */
	UPROPERTY(EditAnywhere, Category = "Abilities|Deflect", meta = (DisplayName = "Info", EditCondition = "!bBegunPlay", HideEditConditionToggle))
	FAbilityInfo DeflectAbilityInfo;

	/** Animation used for the deflect ability. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Deflect", meta = (DisplayName = "Animation Montage"))
	TSoftObjectPtr<UAnimMontage> DeflectMontage;

	/** Index of the ability component used by the deflect ability. */
	int32 DeflectIndex = -1;

	/** List of actors hit by basic attacks. Used to prevent actors from being hit more than once by the same attack. */
	TArray<const AActor*> HitActors;

	/** Name of the montage notify window that controls when the sword collision is enabled and disabled. */
	static const FName DamageWindow;

	/** Name of the montage notify that controls when the player can basic attack again. */
	static const FName CanAttackAgain;

	/** Delegate that triggers when the player lands. */
	FSimpleDelegate OnLandedDelegate;

};
