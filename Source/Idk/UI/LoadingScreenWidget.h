// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <UObject/ObjectMacros.h>

#include "LoadingScreenWidget.generated.h"

/** Widget shown while loading. */
UCLASS(Abstract)
class ULoadingScreenWidget final : public UUserWidget
{
	GENERATED_BODY()
	
};
