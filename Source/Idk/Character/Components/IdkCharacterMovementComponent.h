// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>
#include <UObject/ObjectMacros.h>

#include "IdkCharacterMovementComponent.generated.h"

/** Character movement component used for AIdkCharacter. */
UCLASS(NotBlueprintable)
class UIdkCharacterMovementComponent final : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UIdkCharacterMovementComponent();
};
