// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/LevelScriptActor.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "PersistentLevelScriptActor.generated.h"

class AIdkPlayerCharacter;
class AMapManager;
class UIdkGameInstance;
class ULevelStreamingDynamic;
class ULoadingScreenWidget;
class UMidRunSaveGame;
class UWorld;

/** Actor containing logic for the main level. */
UCLASS()
class APersistentLevelScriptActor final : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	/** Switches to the character select level. */
	void SwitchToCharacterSelect();

	/** Load a saved run. */
	void LoadFromSave(const UMidRunSaveGame& InSave);

	/**
	 * Load the main game level.
	 * 
	 * @param InPlayerClass	Class of the current player character. 
	 */
	void LoadMainLevel(TSubclassOf<AIdkPlayerCharacter> InPlayerClass);

	/** Unload the main level. */
	void UnloadMainLevel();

private:
	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	//~ End AActor Interface

	/** Event called when the player is spawned in the main level. */
	void OnPlayerSpawned();

	/** Event called when the character select level is loaded. */
	UFUNCTION()
	void OnCharacterSelectLoaded();

	/** Event called when the character select level is unloaded. */
	UFUNCTION()
	void OnCharacterSelectUnloaded();

	/** Event called when the lighting for the main level is loaded. */
	UFUNCTION()
	void OnMainLevelLightingLoaded();

	/** Event called when the lighting for the main level is unloaded. */
	UFUNCTION()
	void OnMainLevelLightingUnloaded();

	/** Level for character selection. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftObjectPtr<UWorld> CharacterSelectLevel;

	/** Level with lighting for the main level. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftObjectPtr<UWorld> MainLevelLighting;

	/** Class of the widget to diplay when loading. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<ULoadingScreenWidget> LoadingScreenWidgetClass;

	/** Widget to display when loading. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<ULoadingScreenWidget> LoadingScreenWidget;

	/** Data for streaming the character select level. @see ULevelStreaming */
	UPROPERTY()
	TObjectPtr<ULevelStreamingDynamic> CharacterSelectStreaming;

	/** Data for streaming the level with lighting of the main level. @see ULevelStreaming */
	UPROPERTY()
	TObjectPtr<ULevelStreamingDynamic> MainLevelLightingStreaming;

	/** Class of the current player character. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TSoftClassPtr<AIdkPlayerCharacter> PlayerClass;

	/** Class of the map manager. @see AMapManager */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<AMapManager> MapManagerClass;

	/** Map manager for the current level. @see AMapManager */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<AMapManager> MapManager;

	/** Current game instance. */
	TObjectPtr<UIdkGameInstance> GameInstance;

	/** Current save. */
	TObjectPtr<const UMidRunSaveGame> Save;
};
