// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/Actor.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "CharacterSelectDisplayActor.generated.h"

class UArrowComponent;
class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

/** Actor representing a playable character during character selection. */
UCLASS()
class ACharacterSelectDisplayActor final : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterSelectDisplayActor();

	/** Get transform of the display actor. */
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "Preview Transform") FTransform GetPreviewTransform() const;

private:
	/** Component representing the display actor's transform. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> PreviewTransformComp;

	/** Skeletal mesh component used to display the character's skeletal mesh. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;

#if WITH_EDITORONLY_DATA
	/** Component used to visually represent the preview's transform. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComp;
#endif

};
