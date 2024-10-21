// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/CharacterSelectLevelScriptActor.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/Character/PlayerClasses.h"
#include "Idk/IdkGameInstance.h"
#include "Idk/UI/CharacterSelectWidget.h"
#include <Blueprint/UserWidget.h>
#include <Camera/CameraActor.h>
#include <Components/TimelineComponent.h>
#include <Containers/Array.h>
#include <GameFramework/PlayerController.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Misc/EnumRange.h>
#include <Templates/Casts.h>
#include <Templates/PimplPtr.h>

const FText& FCharacterInfo::GetName() const
{
	return Name;
}

const FText& FCharacterInfo::GetDescription() const
{
	return Description;
}

ACharacterSelectLevelScriptActor::ACharacterSelectLevelScriptActor()
{
	for (const EPlayerClass PlayerClass : TEnumRange<EPlayerClass>())
	{
		CharacterInfo.Add(PlayerClass);
		CharacterClasses.Add(PlayerClass);
		PreviewTransforms.Add(PlayerClass);
	}
}

void ACharacterSelectLevelScriptActor::Init()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	CharacterSelectWidget =
		CreateWidget<UCharacterSelectWidget>(Controller, CharacterSelectWidgetClass.LoadSynchronous());

	TArray<FCharacterDisplayInfo> CharacterDisplayInfo;

	for (const EPlayerClass PlayerClass : TEnumRange<EPlayerClass>())
	{
		const FCharacterInfo& Info = CharacterInfo[PlayerClass];

		CharacterDisplayInfo.Add(FCharacterDisplayInfo(PlayerClass, Info.GetName(), Info.GetDescription()));
	}

	CharacterSelectWidget->Init(CharacterDisplayInfo);

	CharacterSelectWidget->OnSelectedCharacterChangedDelegate.BindUObject(this, &ACharacterSelectLevelScriptActor::OnSelectedCharacterChanged);
	CharacterSelectWidget->OnCharacterChosenDelegate.BindUObject(this, &ACharacterSelectLevelScriptActor::OnCharacterChosen);

	InitPreviewTransforms();

	Camera = GetWorld()->SpawnActor<ACameraActor>();
	Camera->SetActorTransform(GetCameraTransform(PreviewTransforms[EPlayerClass::Knight]));

	Timeline = MakePimpl<FTimeline>();
	Timeline->SetTimelineLength(TimelineLength);
	FOnTimelineFloatStatic TimelineDelegate;
	TimelineDelegate.BindUObject(this, &ACharacterSelectLevelScriptActor::InterpCameraTransform);

	Timeline->AddInterpFloat(TimelineCurve.LoadSynchronous(), TimelineDelegate);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(CharacterSelectWidget->GetCachedWidget());

	Controller->SetViewTarget(Camera);
	Controller->SetInputMode(InputMode);
	CharacterSelectWidget->AddToViewport();
	
	CastChecked<UIdkGameInstance>(GetGameInstance())->CloseMainMenu();
}

void ACharacterSelectLevelScriptActor::InitPreviewTransform(const FTransform& PreviewTransform, const EPlayerClass PlayerClass)
{
	PreviewTransforms[PlayerClass] = PreviewTransform;
}

void ACharacterSelectLevelScriptActor::StartMovingCamera(const FTransform& Start, const FTransform& End)
{
	InterpCameraStartTransform = GetCameraTransform(Start);
	InterpCameraEndTransform = GetCameraTransform(End);

	bCameraMoving = true;

	CharacterSelectWidget->SetButtonsEnabled(false);

	Timeline->PlayFromStart();

	GetWorldTimerManager().SetTimer(Timer, this, &ACharacterSelectLevelScriptActor::TickTimeline, TimelineInterval, true);
}

FTransform ACharacterSelectLevelScriptActor::GetCameraTransform(const FTransform& PreviewTransform) const
{
	const FVector CameraPos = PreviewTransform.GetLocation()
		+ (PreviewTransform.GetRotation().GetForwardVector() * CameraDistance);

	const FRotator CameraRotation = (-1.0 * PreviewTransform.GetRotation().GetForwardVector()).Rotation();

	return FTransform(CameraRotation, CameraPos);
}

void ACharacterSelectLevelScriptActor::OnSelectedCharacterChanged(const EPlayerClass PreviousPlayerClass, const EPlayerClass CurrentPlayerClass)
{
	StartMovingCamera(PreviewTransforms[PreviousPlayerClass], PreviewTransforms[CurrentPlayerClass]);
}

void ACharacterSelectLevelScriptActor::OnCharacterChosen(const EPlayerClass PlayerClass)
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	CharacterSelectWidget->RemoveFromParent();
	Controller->SetShowMouseCursor(false);

	check(!CharacterClasses[PlayerClass].IsNull());

	UIdkGameInstance::GetGameInstance(this)->SetPlayerClass(PlayerClass);

	LoadMainLevelDelegate.Execute(CharacterClasses[PlayerClass].LoadSynchronous());
}

void ACharacterSelectLevelScriptActor::InterpCameraTransform(float T)
{
	const FVector NewPos = 
		FMath::Lerp(InterpCameraStartTransform.GetLocation(), InterpCameraEndTransform.GetLocation(), T);

	const FQuat NewQuat =
		FMath::Lerp(InterpCameraStartTransform.GetRotation(), InterpCameraEndTransform.GetRotation(), T);

	Camera->SetActorTransform(FTransform(NewQuat, NewPos));
}

void ACharacterSelectLevelScriptActor::TickTimeline()
{
	if (Timeline->IsPlaying())
	{
		Timeline->TickTimeline(TimelineInterval);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(Timer);

		CharacterSelectWidget->SetButtonsEnabled(true);
	}
}
