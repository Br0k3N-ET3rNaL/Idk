// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>
#include <UObject/UObjectGlobals.h>

#include "ExperienceComponent.generated.h"

class FDataValidationContext;
class UCurveFloat;
class UExperienceWidget;

DECLARE_DELEGATE_ThreeParams(FOnLevelUpDelegate, const uint8, const uint8, const bool);

/**
 * Component that manages experience and levels.
 */
UCLASS( ClassGroup=(Custom), NotBlueprintable, meta=(BlueprintSpawnableComponent) )
class UExperienceComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UExperienceComponent();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Add experience. Will level up if total experience surpasses the threshold for the next level. */
	void AddExperience(const int32 ExperienceToAdd);

	/** Get the current level. */
	UE_NODISCARD uint8 GetLevel() const;

	/** Get the current amount of experience. */
	UE_NODISCARD int32 GetExperience() const;

	/** Restore saved level and experience. */
	void RestoreLevelAndExperience(const uint8 InLevel, const int32 InExperience);

	/** Set the widget used to display the current level and progress until the next level. */
	void SetExperienceWidget(UExperienceWidget& InExperienceWidget);

	/**
	 * Delegate called when leveling up.
	 * 
	 * @param Level			Current level.
	 * @param LevelsGained	Number of levels gained.
	 */
	FOnLevelUpDelegate OnLevelUpDelegate;

private:
	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	/** Update the experience widget based on current values. */
	void UpdateWidget();

	/**
	 * Get the experience threshold for the next level up.
	 * 
	 * @param CurrentLevel	Current level.
	 * @param Thresholds	Experience thresholds curve.
	 * @return				Experience threshold for the next level up.
	 */
	UE_NODISCARD static int32 GetNextLevelThreshold(const uint8 CurrentLevel, const UCurveFloat& Thresholds);

	/** Amount of experience. */
	UPROPERTY(VisibleInstanceOnly)
	int32 Experience = 0;

	/** Experience needed to reach the next level. */
	UPROPERTY(VisibleInstanceOnly)
	int32 NextThreshold = -1;

	/** Current level. */
	UPROPERTY(VisibleInstanceOnly)
	uint8 Level = 1;

	/** Widget used to display the current level and progress until the next level */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UExperienceWidget> ExperienceWidget;

	/** Curve that determines the amount experience needed to reach the next level. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UCurveFloat> ExperienceThresholds;
	
};
