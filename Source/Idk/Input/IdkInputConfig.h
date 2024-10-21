// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "IdkInputConfig.generated.h"

class UInputAction;

/** Contains all input actions. */
UCLASS()
class UIdkInputConfig final : public UDataAsset
{
	GENERATED_BODY()
	
public:
    /** Input action for movement. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputMove;

    /** Input action for controlling the camera. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputLook;

    /** Input action that activates the player's basic attack. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputBasicAttack;

    /** Input action that activates the player's first ability. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAbility1;

    /** Input action that activates the player's second ability. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAbility2;

    /** Input action that toggles the inventory widget. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> ToggleInventory;

    /** Input action that toggles the full map widget. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> ToggleMenuMap;

    /** Input action for the escape key. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> EscapeKeyPressed;

    /** Input action for interacting with objects in the world. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> Interact;

    /** Input action for zooming in and out on the map widget. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> Zoom;

    /** Input action for panning on the map widget. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> Pan;

    /** Input action that toggles the visiblity of the mouse cursor. */
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction> ToggleMouse;
};
