// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilityGeneratorComponent.generated.h"

class UAbilityChoiceWidget;
class UAbilitySwapWidget;
class UIdkRandomStream;
struct FAbilityInfo;
struct FPlayerSaveData;

DECLARE_DELEGATE_TwoParams(FAddAbilityToPlayerDelegate, const int32, FAbilityInfo&);

/** Component used to randomly generate abilties for the player. */
UCLASS( ClassGroup=(Custom), NotBlueprintable, meta=(BlueprintSpawnableComponent) )
class UAbilityGeneratorComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAbilityGeneratorComponent();

	/**
	 * Initialize the ability generator component.
	 * 
	 * @param Seed			Seed for the random number generator used to generate abilities.
	 * @param InAbilityPool Ability pool to generate abilities from.
	 */
	void Init(const int32 Seed, const TArray<FAbilityInfo*>& InAbilityPool);

	/** Set the widget used to display ability choices to the player. */
	void SetAbilityChoiceWidget(UAbilityChoiceWidget& InAbilityChoiceWidget);

	/** Set the widget that allows the player to equip and swap equipped abilities. */
	void SetAbilitySwapWidget(UAbilitySwapWidget& InAbilitySwapWidget);

	/**
	 * Randomly generate abilities and allow the player to choose between them.
	 * 
	 * @param Num					Number of abilities to generate. Must be a positive number.
	 * @param InExistingAbility1	Pointer to the ability in the player's first ability slot.
	 * @param InExistingAbility2	Pointer to the ability in the player's second ability slot.
	 */
	void GenerateAbilities(const int32 Num, const FAbilityInfo* InExistingAbility1, const FAbilityInfo* InExistingAbility2);

	/**
	 * Event called when an ability is selected by the player.
	 * 
	 * @param GeneratedAbilityIndex Index of the chosen ability among generated abilities.
	 */
	void OnNewAbilityChosen(const int32 GeneratedAbilityIndex);

	/** Called when the player chooses not to equip a generated ability. */
	void DiscardNewAbility();

	/**
	 * Swap an equiped ability for a generated ability.
	 * 
	 * @param AbilityIndex Index of the player's ability slot, either 0 or 1.
	 */
	void SwapNewAbility(const int32 AbilityIndex);

	/** Set the player save data. */
	void SetSaveData(FPlayerSaveData& InSaveData);

	/** Add saved abilities back to player. */
	void RestoreSavedAbilities();

	/**
	 * Delegate called to add the selected ability to the player.
	 * 
	 * @param AbilityIndex	Which player ability slot to use. Either 0 or 1.
	 * @param NewAbility	The ability to add.
	 */
	FAddAbilityToPlayerDelegate AddAbilityToPlayerDelegate;

	/** Delegate that toggles the ability choice widget. */
	FSimpleDelegate ToggleAbilityChoiceWidgetDelegate;

	/** Delegate that toggles the ability swap widget. */
	FSimpleDelegate ToggleAbilitySwapWidgetDelegate;

private:
	/** Random number generator used to generate abilities. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UIdkRandomStream> Rng;

	/** Pool of abilities that can be generated. */
	const TArray<FAbilityInfo*>* AbilityPool = nullptr;

	/** Ability pool indices of generated abilities. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TArray<int32> GeneratedIndices;

	/** Widget used to display ability choices to the player. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UAbilityChoiceWidget> AbilityChoiceWidget;

	/** Widget that allows the player to equip and swap equipped abilities. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UAbilitySwapWidget> AbilitySwapWidget;

	/** Generated ability chosen by player. */
	FAbilityInfo* NewAbility = nullptr;

	/** Ability equipped in the player's first ability slot. */
	const FAbilityInfo* ExistingAbility1 = nullptr;

	/** Ability equipped in the player's second ability slot. */
	const FAbilityInfo* ExistingAbility2 = nullptr;

	/** Player save data. Used to save abilities equipped by the player. */
	FPlayerSaveData* SaveData = nullptr;
		
};
