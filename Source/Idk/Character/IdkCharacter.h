// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/AttackChain.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>
#include <VoxelCharacter.h>

#include "IdkCharacter.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FObjectInitializer;
class UAnimMontage;
class UAttributeSystemComponent;
class UCurveFloat;
class UDamageNumberComponent;
class UHealthAndStatusBarWidget;
class UIdkAnimInstance;
class UInventoryComponent;
class USceneComponent;
struct FPropertyChangedEvent;
struct FBranchingPointNotifyPayload;

/** Default character class used for this project. */
UCLASS(Abstract, NotBlueprintable)
class AIdkCharacter : public AVoxelCharacter
{
	GENERATED_BODY()

public:
	AIdkCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	//~ End AActor Interface

	/** Set the widget used to represent the character's health and display active status effects. */
	virtual void SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget);

	/** Get the vertical offset from the root component to the character's feet. Used when spawning. */
	UE_NODISCARD double GetSpawnZOffset() const;

	/** Get the characters attribute system component. */
	UE_NODISCARD UAttributeSystemComponent* GetAttributeSystem() const;

protected:
	/** 
	 * Get the character's current level.
	 * 
	 * Needs to be overriden by direct subclasses.
	 */
	UE_NODISCARD virtual uint8 GetLevel() const PURE_VIRTUAL(&AIdkCharacter::GetLevel, return 1;);

	/** Event fired when the character dies. */
	UFUNCTION()
	virtual void OnDeath();

	/** Event handler for AnimInstance's OnPlayMontageNotifyBegin. */
	UFUNCTION()
	virtual void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Event handler for AnimInstance's OnPlayMontageNotifyEnd. */
	UFUNCTION()
	virtual void OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Event handler for AnimInstance's OnMontageEnded. Subclasses should call Super::OnMontageEnded if overriding. */
	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** Event handler for AnimInstance's OnMontageBlendingOut. */
	UFUNCTION()
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Attribute system component that handles interaction with many game systems. @see UAttributeSystemComponent */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeSystemComponent> AttributeSystemComponent;

	/** Death animation used by the character. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UAnimMontage> DeathAnimation;

	/** Ability info for character's basic attack */
	UPROPERTY(EditAnywhere, Category = "Basic Attack", meta = (DisplayName = "Info"))
	FAbilityInfo BasicAttackAbilityInfo;

	/** Animations used as a part of a character's basic attack chain. @see FAttachChain */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (DisplayName = "Attack Chain"))
	FAttackChain BasicAttackChain;

	/** Pool of abilities useable by the character. */
	TArray<FAbilityInfo*> AbilityPool;

	/** AnimInstance associated with the character's skeletal mesh component. */
	UPROPERTY()
	TObjectPtr<UIdkAnimInstance> AnimInstance;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	bool bBegunPlay = false;
#endif

private:
	/** Component that creates floating numbers for damage and healing. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDamageNumberComponent> DamageNumberComponent;

};
