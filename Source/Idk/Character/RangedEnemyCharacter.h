// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkEnemyCharacter.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "RangedEnemyCharacter.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FObjectInitializer;
class UAbilityComponent;
class UAnimMontage;
class UParticleSystem;

/** Base class for enemies that use a beam-based basic attack. */
UCLASS()
class ARangedEnemyCharacter : public AIdkEnemyCharacter
{
	GENERATED_BODY()
	
public:
	ARangedEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin AIdkCharacter Interface.
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	//~ End AIdkCharacter Interface

	//~ Begin AIdkEnemyCharacter Interface.
	virtual void BasicAttack() override;
	//~ End AIdkEnemyCharacter Interface

	/** Event called when the basic attack finishes charging. */
	UFUNCTION()
	void OnBasicAttackChargeEnd();

	/** Event called when the basic attack finishes firing. */
	void OnBasicAttackFireEnd();

	/** Gets the names of all sockets on the character's skeletal mesh. */
	UFUNCTION()
	TArray<FName> GetSocketNames() const;

	/** Beam VFX used by the basic attack. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (DisplayName = "Beam"))
	TSoftObjectPtr<UParticleSystem> BasicAttackBeam;

	/** Time it takes to charge a basic attack before firing. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (ClampMin = "0.0"))
	double ChargeTime = 1.0;

	/** Name of the socket that represents the muzzle of the enemy's weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Basic Attack", meta = (GetOptions = "GetSocketNames"))
	FName MuzzleSocket = NAME_None;
};
