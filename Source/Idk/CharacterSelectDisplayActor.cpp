// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/CharacterSelectDisplayActor.h"

#include <Components/ArrowComponent.h>
#include <Components/SceneComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/CoreMiscDefines.h>

ACharacterSelectDisplayActor::ACharacterSelectDisplayActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	PreviewTransformComp = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewTransformComp"));

	RootComponent = SkeletalMeshComp;
	PreviewTransformComp->SetupAttachment(SkeletalMeshComp);

#if WITH_EDITORONLY_DATA
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(PreviewTransformComp);
#endif
}

FTransform ACharacterSelectDisplayActor::GetPreviewTransform() const
{
	return PreviewTransformComp->GetComponentTransform();
}
