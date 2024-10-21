// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/ConfirmationBox.h"

#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/GameSystems/OrientToPlayerComponent.h"
#include "Idk/UI/ConfirmationWidget.h"
#include <Components/WidgetComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

AConfirmationBox::AConfirmationBox()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the component that will ensure that the widget always faces the player
	UOrientToPlayerComponent* OrientToPlayerComp = 
		CreateDefaultSubobject<UOrientToPlayerComponent>(TEXT("OrientToPlayerComp"));
	OrientToPlayerComp->Init(false);

	RootComponent = OrientToPlayerComp;
	ConfirmationWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ConfirmationWidgetComp"));

	ConfirmationWidgetComp->SetupAttachment(RootComponent);
	ConfirmationWidgetComp->SetRelativeLocation(FVector(0.0, 0.0, AIdkPlayerCharacter::GetInteractionHeight()));

	// Draw the widget at a higher resolution
	ConfirmationWidgetComp->SetDrawSize(DrawSize * UpscaleFactor);

	// Scale the component down to maintain the same widget size
	ConfirmationWidgetComp->SetRelativeScale3D(FVector(1.0 / UpscaleFactor));

	ConfirmationWidgetComp->SetWindowFocusable(false);
	ConfirmationWidgetComp->bReceivesDecals = false;
	ConfirmationWidgetComp->SetTickWhenOffscreen(false);
}

#if WITH_EDITOR
EDataValidationResult AConfirmationBox::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (ConfirmationWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Confirmation widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (DrawSize.X <= 0.0 || DrawSize.Y <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("Draw size should be positive.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AConfirmationBox::SetMessage(const FText& Message)
{
	check(!ConfirmationWidgetClass.IsNull());

	ConfirmationWidgetComp->SetWidgetClass(ConfirmationWidgetClass.LoadSynchronous());

	UConfirmationWidget* ConfirmationWidget = CastChecked<UConfirmationWidget>(ConfirmationWidgetComp->GetWidget());
	ConfirmationWidget->SetMessage(Message);
	ConfirmationWidget->OnOptionSelectedDelegate.BindLambda([&](const bool bConfirmed)
		{
			OnOptionSelectedDelegate.ExecuteIfBound(bConfirmed);
		});
}
