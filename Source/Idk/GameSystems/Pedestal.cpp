// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Pedestal.h"

#include "Idk/UI/GenericDisplayInfo.h"
#include "Idk/UI/PedestalDisplayInfo.h"
#include "Idk/UI/PedestalDisplayWidget.h"
#include "Idk/UI/TooltipWidget.h"
#include <Components/WidgetComponent.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/UObjectGlobals.h>

APedestal::APedestal()
{
	PrimaryActorTick.bCanEverTick = false;

	PedestalDisplayWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PedestalDisplayWidgetComp"));
	TooltipWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("TooltipWidgetComp"));

	RootComponent = PedestalDisplayWidgetComp;
	TooltipWidgetComp->SetupAttachment(PedestalDisplayWidgetComp);

	const double PedestalSize = PedestalDisplayDrawSize * PedestalDisplayUpscaleFactor;

	PedestalDisplayWidgetComp->SetDrawSize(FVector2D(PedestalSize));
	PedestalDisplayWidgetComp->SetRelativeScale3D(FVector(1.0 / PedestalDisplayUpscaleFactor));
	PedestalDisplayWidgetComp->SetWindowFocusable(false);
	PedestalDisplayWidgetComp->bReceivesDecals = false;
	PedestalDisplayWidgetComp->SetTickWhenOffscreen(false);

	const double TooltipOffset = (-PedestalSize / 2.0) - (10.0 * PedestalDisplayUpscaleFactor);

	TooltipWidgetComp->SetVisibility(false);
	TooltipWidgetComp->SetRelativeLocation(FVector(0.0, TooltipOffset, 0.0));
	TooltipWidgetComp->SetPivot(FVector2D(0.0, 0.5));
	TooltipWidgetComp->SetDrawAtDesiredSize(true);
	TooltipWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
}

#if WITH_EDITOR
EDataValidationResult APedestal::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (PedestalDisplayWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Pedestal display widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (TooltipWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Tooltip widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void APedestal::Init(const FPedestalDisplayInfo& DisplayInfo)
{
	check(!PedestalDisplayWidgetClass.IsNull());
	check(!TooltipWidgetClass.IsNull());

	TooltipWidgetComp->SetWidgetClass(TooltipWidgetClass.LoadSynchronous());
	UTooltipWidget* TooltipWidget = CastChecked<UTooltipWidget>(TooltipWidgetComp->GetWidget());

	const FGenericDisplayInfo& Info = DisplayInfo.GetDisplayInfo();

	TooltipWidget->Init(Info.GetDisplayName(), Info.GetDescription());

	// Update the tooltip whenever it is changed
	Info.UpdateDescriptionDelegate.AddUObject(TooltipWidget, &UTooltipWidget::UpdateDescription);

	PedestalDisplayWidgetComp->SetWidgetClass(PedestalDisplayWidgetClass.LoadSynchronous());

	UPedestalDisplayWidget* PedestalDisplayWidget =
		CastChecked<UPedestalDisplayWidget>(PedestalDisplayWidgetComp->GetWidget());

	PedestalDisplayWidget->OnHoverBeginDelegate.BindUObject(this, &APedestal::OnHover);
	PedestalDisplayWidget->OnHoverEndDelegate.BindUObject(this, &APedestal::OnHoverEnd);
	PedestalDisplayWidget->OnSelectedDelegate.BindLambda([&]() {
		PedestalSelectedDelegate.ExecuteIfBound();
		});

	PedestalDisplayWidget->Init(DisplayInfo);
}

void APedestal::OnHover()
{
	// Show the tooltip
	TooltipWidgetComp->SetVisibility(true);
	TooltipWidgetComp->SetWindowVisibility(EWindowVisibility::Visible);
}

void APedestal::OnHoverEnd()
{
	// Hide the tooltip
	TooltipWidgetComp->SetVisibility(false);
	TooltipWidgetComp->SetWindowVisibility(EWindowVisibility::SelfHitTestInvisible);
}
