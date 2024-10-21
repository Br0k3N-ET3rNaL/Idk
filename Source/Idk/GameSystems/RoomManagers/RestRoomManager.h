// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "RestRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AHealPickup;
class FDataValidationContext;
class UCurveFloat;

/** Room that provides the player the chance to heal. */
UCLASS(Blueprintable)
class ARestRoomManager final : public AHallRoomManager
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

private:
	//~ Begin ARoomManager Interface.
	virtual void OnRoomActivated() override;
	virtual void OnCompletion() override;
	//~ End ARoomManager Interface

	/** Blueprint class that heals the player when picked up. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<AHealPickup> HealPickupClass;

	/** Curve that determines the amount healed based on the room's level. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UCurveFloat> HealingCurve;

	/** Heals the player when picked up. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AHealPickup> HealPickup;
};
