// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "PedestalSelector.generated.h"

enum class EDataValidationResult : uint8;
class APedestal;
class FDataValidationContext;
class UChildActorComponent;
class USceneComponent;
struct FGenericDisplayInfo;
struct FPedestalDisplayInfo;

DECLARE_DELEGATE_OneParam(FPedestalSelectedDelegate, uint8);

/** Creates a number of pedestals that the player can select. */
UCLASS(Abstract)
class APedestalSelector : public AActor
{
	GENERATED_BODY()
	
public:	
	APedestalSelector();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * Initialize the pedestal selector.
	 *
	 * @param PedestalDisplayInfo	Information to display for each pedestal.
	 */
	void Init(const TArray<FPedestalDisplayInfo>& PedestalDisplayInfo);

	/** Get the number of pedestals used by the pedestal selector. */
	UE_NODISCARD uint8 GetNumPedestals() const;

	/**
	 * Delegate called when a pedestal is selected.
	 * 
	 * @param Index	Index of the selected pedestal.
	 */
	FPedestalSelectedDelegate PedestalSelectedDelegate;

protected:
	/**
	 * Event called when a pedestal is selected.
	 *
	 * @param Index	Index of the selected pedestal.
	 */
	virtual void OnPedestalSelected(const uint8 Index);

	/** Blueprint class for the pedestals. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<APedestal> PedestalClass;

	/** Number of pedestals. */
	UPROPERTY(EditDefaultsOnly)
	uint8 NumPedestals = 3;

	/** Currently created pedestals. */
	UPROPERTY(VisibleAnywhere)
	TArray<APedestal*> Pedestals;

	/** Horizontal distance between pedestals. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = "cm"))
	double PedestalSpacing = 75.0;

	/** Height to display the pedestals at. */
	UPROPERTY(VisibleAnywhere, meta = (Units = "cm"))
	double PedestalDisplayHeight;

};
