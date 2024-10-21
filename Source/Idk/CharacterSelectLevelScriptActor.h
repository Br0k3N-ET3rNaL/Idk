// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/PlayerClasses.h"
#include <Containers/Map.h>
#include <Delegates/DelegateCombinations.h>
#include <Engine/LevelScriptActor.h>
#include <Engine/TimerHandle.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Templates/PimplPtr.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "CharacterSelectLevelScriptActor.generated.h"

class ACameraActor;
class AIdkPlayerCharacter;
class UCharacterSelectWidget;
class UCurveFloat;
class UMainMenuWidget;
struct FTimeline;

DECLARE_DELEGATE_OneParam(FLoadMainLevelDelegate, TSubclassOf<AIdkPlayerCharacter>);

/** Information representing a playable character class. */
USTRUCT()
struct FCharacterInfo final
{
	GENERATED_BODY()

public:
	/** Get the character's name. */
	UE_NODISCARD const FText& GetName() const;

	/** Get the description of the character class. */
	UE_NODISCARD const FText& GetDescription() const;

private:
	/** Character's name. */
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	/** Description of the character class. */
	UPROPERTY(EditDefaultsOnly, meta = (MultiLine))
	FText Description;

};

/** Actor containing logic for the character select level. */
UCLASS()
class ACharacterSelectLevelScriptActor final : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ACharacterSelectLevelScriptActor();

	/** Initialize the character select level. */
	void Init();

	/**
	 * Delegate called to load the main level.
	 * 
	 * @param PlayerClass	Class of the selected character class.
	 */
	FLoadMainLevelDelegate LoadMainLevelDelegate;

protected:
	/** Blueprint implementable function that should initialize the preview transforms for each character select actor. */
	UFUNCTION(BlueprintImplementableEvent, meta = (DefaultToSelf, HideSelfPin))
	void InitPreviewTransforms();

	/**
	 * Initialize the preview transform for a character select actor.
	 * 
	 * @param PreviewTransform	Transform of the character select actor. 
	 * @param PlayerClass		Class the character select actor represents.	
	 */
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf, HideSelfPin))
	void InitPreviewTransform(const FTransform& PreviewTransform, const EPlayerClass PlayerClass = EPlayerClass::Knight);

private:
	/**
	 * Start moving the camera.
	 * 
	 * @param Start	Transform to start from. 
	 * @param End	Transform to move to.
	 */
	void StartMovingCamera(const FTransform& Start, const FTransform& End);

	/** Get the transform for a camera looking at an actor with the specified transform. */
	UE_NODISCARD FTransform GetCameraTransform(const FTransform& PreviewTransform) const;

	/**
	 * Event called when the selected character changes.
	 * 
	 * @param PreviousPlayerClass	Previous character class. 
	 * @param CurrentPlayerClass	Current character class.
	 */
	void OnSelectedCharacterChanged(const EPlayerClass PreviousPlayerClass, const EPlayerClass CurrentPlayerClass);

	/**
	 * Event called when a character class is chosen.
	 * 
	 * @param PlayerClass	Class of the character chosen. 
	 */
	void OnCharacterChosen(const EPlayerClass PlayerClass);

	/** Interpolate the transform of the camera between two transforms. @see StartMovingCamera() */
	void InterpCameraTransform(float T);

	/** Function called by a timer to update the timeline used to interpolate the camera's transform. */
	UFUNCTION()
	void TickTimeline();

	/** Curve used to interpolate the camera's transform. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UCurveFloat> TimelineCurve;

	/** How often to update the camera's transform when interpolating. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (Units = "s", ClampMin = 0.01))
	double TimelineInterval = 0.25;

	/** How long to interpolate the camera's transform. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (Units = "s", ClampMin = 0.01))
	double TimelineLength = 2.0;

	/** Distance of the camera from a character select actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (Units = "cm"))
	double CameraDistance = 200.0;

	/** Class of the widget used to display information about the currently selected character class. @see UCharacterSelectWidget */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<UCharacterSelectWidget> CharacterSelectWidgetClass;

	/** Widget used to display information about the currently selected character class. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UCharacterSelectWidget> CharacterSelectWidget;

	/** Class of the widget used to display the main menu for the game. @see UMainMenuWidget */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (BlueprintBaseOnly))
	TSoftClassPtr<UMainMenuWidget> MainMenuWidgetClass;

	/** Widget used to display the main menu for the game. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UMainMenuWidget> MainMenuWidget;

	/** Maps playable character classes to information about that character class. @see FCharacterInfo */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ReadOnlyKeys, ForceInlineRow))
	TMap<EPlayerClass, FCharacterInfo> CharacterInfo;

	/** Maps playable character classes to the class of the actor for that character class. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ReadOnlyKeys, ForceInlineRows, BlueprintBaseOnly))
	TMap<EPlayerClass, TSoftClassPtr<AIdkPlayerCharacter>> CharacterClasses;

	/** Maps playable character classes to the transform of the character select actor for that character class. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk", meta = (ReadOnlyKeys, ForceInlineRow))
	TMap<EPlayerClass, FTransform> PreviewTransforms;

	/** Camera facing the currently selected character. */
	UPROPERTY()
	TObjectPtr<ACameraActor> Camera;

	/** Timeline used to interpolate the camera's transform. */
	TPimplPtr<FTimeline> Timeline;

	/** Timer used to update the timer used to interpolate the camera's transform. */
	FTimerHandle Timer;

	/** Transform to start interpolating the camera's transform from. */
	FTransform InterpCameraStartTransform;

	/** Transform to interpolate the camera's transform to. */
	FTransform InterpCameraEndTransform;

	/** Whether the camera is currently moving. */
	bool bCameraMoving = false;
	
};
