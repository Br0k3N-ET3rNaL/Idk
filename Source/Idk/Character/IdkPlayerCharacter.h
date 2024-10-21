// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/IdkCharacter.h"
#include "Idk/Character/PlayerSaveData.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Delegates/Delegate.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "IdkPlayerCharacter.generated.h"

enum class EDataValidationResult : uint8;
class AIdkPlayerController;
class FDataValidationContext;
class FName;
class FObjectInitializer;
class UAbilityCollisionComponent;
class UAbilityComponent;
class UAbilityGeneratorComponent;
class UAbilityIndicatorComponent;
class UAbilityWidget;
class UCameraComponent;
class UCharacterMovementComponent;
class UExperienceComponent;
class UGoldComponent;
class UHealthAndStatusBarWidget;
class UIdkInputConfig;
class UInputComponent;
class UInputMappingContext;
class UItem;
class UInventoryWidget;
class UMapWidget;
class UParticleSystem;
class UPlayerAbilityComponent;
class UPlayerAssetBindings;
class UPlayerInventoryComponent;
class USpringArmComponent;
class UStatusEffect;
class UStatusEffectBlueprintBase;
class UWidgetInteractionComponent;
struct FAbilityInfo;
struct FGameplayTag;
struct FInputActionValue;
struct FMapInitInfo;
struct FPlayerWidgets;

/** Character subclass used for players. */
UCLASS(Abstract)
class AIdkPlayerCharacter : public AIdkCharacter
{
	GENERATED_BODY()

public:
	AIdkPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin APawn Interface.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	//~ End APawn Interface

	/** Connects HUD widgets to various components. Called after widgets are created by the player controller. */
	void SetWidgets(UHealthAndStatusBarWidget& HealthAndStatusBar, FPlayerWidgets& PlayerWidgets);
	
	/**
	 * Initializes the map widget.
	 * Called by the map manager after map generation has completed.
	 * @param MapInitInfo Struct containing information needed to initialize the map widget.
	 * @see FMapInitInfo
	 */
	void InitMapWidget(const FMapInitInfo& MapInitInfo);

	/** Set the seed used by the ability generator component. */
	void InitSeed(const int32 Seed);

	/** Allow the player to choose an ability at the start of a run. */
	void GenerateStartingAbility();

	/** Gets the forward vector of the player's camera. */
	UE_NODISCARD FVector GetCameraForwardVector() const;

	/** Gets the location of the player's camera. */
	UE_NODISCARD FVector GetCameraLocation() const;

	/** Gets the rotation of the player's camera */
	UE_NODISCARD FQuat GetCameraRotation() const;

	/** Affects movement, basic attack, and ability inputs. */
	void SetInputEnabled(bool bInInputEnabled);

	/** Add an item to the player's inventory. */
	void AddItemToInventory(UItem& Item);

	/** Adds saved items to the player's inventory. */
	void RestoreInventory(const TArray<UItem*>& Items);

	/**
	 * Gets the item tags of items in the player's inventory, along with the
	 * number of items with each tag.
	 * 
	 * Used by the item generator to weight items based on item's the player has.
	 * 
	 * @param OutItemTagCounts TMap where the key is an item tag, and the value is number of items with that tag.
	 * @param OutItemCount Total number of items.
	 */
	void GetItemTagCounts(TMap<FGameplayTag, int32>& OutItemTagCounts, int32& OutItemCount) const;

	/** Add experience to the player. */
	void AddExperience(const int32 Experience);

	/** Add gold to the player */
	void AddGold(const int32 Gold);

	/** Checks if the player has enough gold to afford something with the specified price. */
	bool CanAfford(const int32 Price) const;

	/** Removes gold from the player. Used primarily when buying itmes. */
	void RemoveGold(const int32 Gold);

	/** Event called when the final boss has been killed. Ends the game. */
	void OnBossKilled();

	/** Event called when the player is possessed by the player controller. */
	void OnPossessed(AIdkPlayerController* InPlayerController);

	/** Gets save data associated with the player. @see FPlayerSaveData */
	UE_NODISCARD const FPlayerSaveData& GetSaveData() const;

	/** Restores the player state from save data. */
	void RestoreSaveData(const FPlayerSaveData& InSaveData, const TArray<UItem*>& SavedItems);

	/** Get the height to use for interactable widgets. */
	UE_NODISCARD static double GetInteractionHeight();

	/** Delegate called when the player is possessed by the player controller. */
	FSimpleDelegate OnPossessedDelegate;

	/** Delegate called when the player is killed. */
	FSimpleDelegate OnPlayerDeathDelegate;

protected:
	//~ Begin AIdkCharacter Interface.
	UE_NODISCARD virtual uint8 GetLevel() const override final;
	virtual void OnDeath() override;
	//~ End AIdkCharacter Interface

	/** Implementation of the player's basic attack. Must be overriden by subclasses. */
	UFUNCTION()
	virtual void BasicAttackImpl() PURE_VIRTUAL(&AIdkPlayerCharacter::BasicAttackImpl, ;);

	/** Ability component used for player basic attacks. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerAbilityComponent> BasicAttackAbilityComp;

	/** Maps indexes of abilities in the ability pool to the ability component using that ability. */
	TMap<int32, UPlayerAbilityComponent*> AbilityComps;

private:
	/**
	 * Event called when the player levels up. 
	 * 
	 * @param NewLevel The level of the player after leveling up.
	 * @param LevelsGained The number of levels gained.
	 * @param bRestoringFromSave True when level up occurs due to restoring from save.
	 */
	void OnLevelUp(const uint8 NewLevel, const uint8 LevelsGained, const bool bRestoringFromSave);

	/**
	 * Gives the specified ability to the player.
	 * @param AbilityCompIndex Which ability component to use. Either 0 or 1.
	 * @param NewAbility The ability to add.
	 */
	void AddAbility(const int32 AbilityCompIndex, FAbilityInfo& NewAbility);

	/** Handles movement input. */
	void Move(const FInputActionValue& Value);

	/** Handles look input. */
	void Look(const FInputActionValue& Value);

	/** Handles input for first ability activation. */
	void ActivateAbility1(const FInputActionValue& Value);

	/** Handles input for second ability activation. */
	void ActivateAbility2(const FInputActionValue& Value);

	/** Handles input for showing the indicator for the first ability. */
	void ShowAbility1Indicator(const FInputActionValue& Value);

	/** Handles input for showing the indicator for the second ability. */
	void ShowAbility2Indicator(const FInputActionValue& Value);

	/** Handles interaction input. */
	void Interact(const FInputActionValue& Value);

	/** Handles input for toggling the inventory widget. */
	void ToggleInventory(const FInputActionValue& Value);

	/** Handles input for toggling the full map widget. */
	void ToggleMenuMap(const FInputActionValue& Value);

	/** Handles escape key input. */
	void EscapeKeyPressed(const FInputActionValue& Value);

	/** Handles input for map zoom. */
	void Zoom(const FInputActionValue& Value);

	/** Handles input for map panning. */
	void Pan(const FInputActionValue& Value);

	/** Handles basic attack input. */
	void BasicAttack(const FInputActionValue& Value);

	/** Handles input for toggling the visibility of the mouse cursor. */
	void ToggleMouse(const FInputActionValue& Value);

	/**
	 * Disables an item from the player's inventory. 
	 * 
	 * @param ItemName The name of the item to disable.
	 */
	void DisableItem(const FName& ItemName);

	/** Player's third person camera. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

	/** Spring arm component used for the camera. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	/** Component used for interacting with in-world widgets. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetInteractionComponent> WidgetInteractionComp;

	/** Ability component for the player's first ability. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerAbilityComponent> AbilityComp1;

	/** Ability component for the player's second ability. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerAbilityComponent> AbilityComp2;

	/** Component that manages the player's items. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerInventoryComponent> InventoryComp;

	/** Component that handles gaining experience and levelling up. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UExperienceComponent> ExperienceComp;

	/** Component that manages the player's gold. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGoldComponent> GoldComp;

	/** Component used to randomly generate abilities for the player. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilityGeneratorComponent> AbilityGeneratorComp;

	/** Data asset containing all input actions used by the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UIdkInputConfig> InputActions;

	/** Controller possessing the player. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<AIdkPlayerController> PlayerController;

	/** When false, blocks movement, basic attack, and ability input. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	bool bInputEnabled = true;

	/** Class of the status effect to apply when leveling up. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftClassPtr<UStatusEffectBlueprintBase> LevelUpBonusClass;

	/** Status effect to apply when leveling up. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UStatusEffect> LevelUpBonus;

	/** Level where a second abiity will be offered. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ClampMin = "2"))
	int32 SecondAbilityLevel = 3;

	/** The player will have the option to swap abilities every X levels after Second Ability Level. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ClampMin = "1"))
	int32 GainAbilityInterval = 5;

	/** The number of abilties to choose between when the player levels up. Large values may not display properly. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ClampMin = "0"))
	int32 AbilityChoicesOnLevelUp = 2;

	/** Player related save data. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	mutable FPlayerSaveData SaveData;

	/** VFX to display when levelling up. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UParticleSystem> LevelUpVFX;

	/** Height to use for interactable widgets. */
	static constexpr double InteractionHeight = 160;
};
