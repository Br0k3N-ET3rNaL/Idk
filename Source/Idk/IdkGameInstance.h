// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/PlayerClasses.h"
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <Engine/GameInstance.h>
#include <HAL/Platform.h>
#include <Templates/Function.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "IdkGameInstance.generated.h"

enum class EDataValidationResult : uint8;
enum class EItemRarity : uint8;
enum class EPathTaken : uint8;
class AGameInstanceActor;
class AIdkPlayerCharacter;
class FDataValidationContext;
class FString;
class FText;
class UCurveFloat;
class UEnhancedInputUserSettings;
class UHealthAndStatusBarWidget;
class UHUDWidget;
class UItemPool;
class UItemGenerator;
class UMainMenuWidget;
class UMaterialInterface;
class UMidRunSaveGame;
class UNiagaraSystem;
class UObject;
class UParticleSystem;
class UStatusEffectTable;
struct FItemSaveData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSetDelegate, AIdkPlayerCharacter*);
DECLARE_DELEGATE_OneParam(FShowTempMessageDelegate, const FText&);

/**  */
UCLASS()
class UIdkGameInstance final : public UGameInstance
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin UGameInstance Interface.
	virtual void OnStart() override;
	//~ End UGameInstance Interface

	/** Get the current item generator. */
	UE_NODISCARD UItemGenerator& GetItemGenerator() const;

	/** Get the material used by ability indicators. @see UAbilityIndicatorComponent */
	UE_NODISCARD UMaterialInterface& GetAbilityIndicatorMaterial() const;

	/** Get the particle system used for damage numbers. @see UDamageNumberComponent */
	UE_NODISCARD UNiagaraSystem& GetDamageNumberParticleSystem() const;

	/** Get the VFX that is displayed when enemies are spawned. */
	UE_NODISCARD UParticleSystem& GetSpawnVFX() const;

	/** Get the class of the widget for displaying health and status effects for enemies. */
	UE_NODISCARD TSubclassOf<UHealthAndStatusBarWidget> GetHealthAndStatusBarWidgetClass() const;

	/** Get the curve used to interpolate arena's door's position when opening and closing. */
	UE_NODISCARD UCurveFloat& GetDoorOpenCurve() const;

	/** Set the class of the current player. */
	void SetPlayerClass(const EPlayerClass PlayerClass);

	/** Set the current player. */
	void SetPlayer(AIdkPlayerCharacter& InPlayer);

	/** Get the current player. */
	UE_NODISCARD AIdkPlayerCharacter* GetPlayer() const;

	/** Event called after play has begun. */
	void OnBeginPlay();

	/** Close the main menu. */
	void CloseMainMenu();

	/** Event called when the player dies. */
	void OnPlayerDeath();

	/** Return to the main menu. */
	void ReturnToMainMenu();

	/** Event called when the current level is complete. */
	void OnLevelComplete();

	/** Create a new mid-run save. */
	void CreateNewMidRunSave();

	/** Save the current map's seed. */
	void SaveMapSeed(const int32 Seed);

	/** Save the current player's class. */
	void SavePlayerClass(const TSubclassOf<AIdkPlayerCharacter> PlayerClass);

	/**
	 * Saves the player's progress through the map.
	 *
	 * @param PathTaken	Represents the path the player took.
	 */
	void UpdatePlayerProgress(const EPathTaken PathTaken);

	/** Save an item that was removed from the item pool. */
	void SaveRemovedItem(const FItemSaveData& ItemSaveData);

	/** Delete the current mid-run save. */
	void ClearMidRunSave();

	/** Event called when saving has completed. */
	void OnSaveComplete();

	/** Check if saving is in progress. */
	bool IsSaving() const;

	/** Show a temporary message to the player. */
	void ShowTempMessage(const FText& Message) const;

	/**
	 * Begin exiting the game.
	 * 
	 * @param Callback	Function to call before exiting. 
	 */
	void RequestExit(TFunction<void(void)>&& Callback = nullptr);

	/**
	 * Begin exiting to the main menu.
	 * 
	 * @param Callback	Function to call before exiting to the main menu.
	 */
	void RequestExitToMenu(TFunction<void(void)>&& Callback = nullptr);

	/** Get the current game instance. */
	UE_NODISCARD static UIdkGameInstance* GetGameInstance(const UObject* WorldContextObject);

	/**
	 * Delegate called when the player is set.
	 * 
	 * @param Player	The current player character.
	 */
	FOnPlayerSetDelegate OnPlayerSetDelegate;

private:
	/** Save the mid-run save. */
	void SaveGame();

	/** Initialize the mid-run save. */
	void InitSaveGame();

	/** Exit the game. */
	void ExitGame();

	/** Class of the widget for the main menu. @see UMainMenuWidget */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<UMainMenuWidget> MainMenuWidgetClass;

	/** Widget for the main menu. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;

	/** Class of the item generator. @see UItemGenerator */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UItemGenerator> ItemGeneratorClass;

	/** Material used by ability indicators. @see UAbilityIndicatorComponent */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UMaterialInterface> AbilityIndicatorMaterial;

	/** Particle system used for floating damage numbers. @see UDamageNumberComponent */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UNiagaraSystem> DamageNumberParticleSystem;

	/** VFX displayed when enemies are spawned. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UParticleSystem> SpawnVFX;

	/** Table of all non-unique status effects. @see UStatusEffectTable */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UStatusEffectTable> StatusEffectTable;

	/** Class of the widget for displaying health and status effects for enemies. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UHealthAndStatusBarWidget> HealthAndStatusBarWidgetClass;

	/** Curve used to interpolate arena's door's position when opening and closing. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TObjectPtr<UCurveFloat> DoorOpenCurve;

	/** Current item generator. @see UItemGenerator */
	UPROPERTY(VisibleInstanceOnly, meta = (ShowInnerProperties))
	TObjectPtr<UItemGenerator> ItemGenerator;

	/** Current mid-run save. @see UMidRunSaveGame */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMidRunSaveGame> MidRunSave;

	/** Items to add to the player when spawning. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TArray<FItemSaveData> StarterItemData;

	/** Current player character. */
	TObjectPtr<AIdkPlayerCharacter> Player;

	/** Actor that holds a reference to the game instance. @see AGameInstanceActor */
	TObjectPtr<AGameInstanceActor> GameInstanceActor;

	/**
	 * Delegate called to display a temporary message to the player.
	 * 
	 * @param Message	Message to display.
	 */
	FShowTempMessageDelegate ShowTempMessageDelegate;

	/** Whether to exit the game after saving is complete. */
	bool bExitAfterSaving = false;

	/** Whether to exit to the main menu after saving is complete. */
	bool bReturnToMenuAfterSaving = false;

	/** Function to call before exiting the game or exiting to the main menu. */
	TFunction<void(void)> PreExitCallback;

};
