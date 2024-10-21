// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/CharacterSelectWidget.h"

#include "Idk/Character/PlayerClasses.h"
#include "Idk/UI/CharacterSelectInfoWidget.h"
#include "Idk/UI/CommonButton.h"
#include <Blueprint/UserWidget.h>
#include <CommonWidgetCarousel.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <UObject/UObjectGlobals.h>

FCharacterDisplayInfo::FCharacterDisplayInfo(const EPlayerClass PlayerClass, const FText& Name, const FText& Description)
	: PlayerClass(PlayerClass), Name(Name), Description(Description)
{
	check(PlayerClass != EPlayerClass::None);
	check(PlayerClass != EPlayerClass::Num);

	bCanSelectPrevious = (uint8(PlayerClass) - 1) > uint8(EPlayerClass::None);
	bCanSelectNext = (uint8(PlayerClass) + 1) < uint8(EPlayerClass::Num);
}

EPlayerClass FCharacterDisplayInfo::GetPlayerClass() const
{
	return PlayerClass;
}

const FText& FCharacterDisplayInfo::GetName() const
{
	return Name;
}

const FText& FCharacterDisplayInfo::GetDescription() const
{
	return Description;
}

bool FCharacterDisplayInfo::CanSelectPrevious() const
{
	return bCanSelectPrevious;
}

bool FCharacterDisplayInfo::CanSelectNext() const
{
	return bCanSelectNext;
}

#if WITH_EDITOR
EDataValidationResult UCharacterSelectWidget::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (InfoWidgetClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Info widget class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UCharacterSelectWidget::Init(const TArray<FCharacterDisplayInfo>& InCharacterDisplayInfo)
{
	check(!InfoWidgetClass.IsNull());
	check(InCharacterDisplayInfo.Num() > 0);

	CharacterDisplayInfo = InCharacterDisplayInfo;

	for (const FCharacterDisplayInfo& DisplayInfo : CharacterDisplayInfo)
	{
		UCharacterSelectInfoWidget* InfoWidget
			= CreateWidget<UCharacterSelectInfoWidget>(this, InfoWidgetClass.LoadSynchronous());

		InfoWidget->Init(DisplayInfo.GetName(), DisplayInfo.GetDescription());

		CharacterInfoCarousel->AddChild(InfoWidget);
	}

	CharacterInfoCarousel->SetActiveWidgetIndex(0);

	UpdateButtons();
}

void UCharacterSelectWidget::SetButtonsEnabled(const bool bEnabled)
{
	if (bEnabled)
	{
		UpdateButtons();
	}
	else
	{
		PreviousButton->SetIsEnabled(false);
		NextButton->SetIsEnabled(false);
	}

	ConfirmButton->SetIsEnabled(bEnabled);
}

void UCharacterSelectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PreviousButton->OnClicked().AddUObject(this, &UCharacterSelectWidget::OnPreviousButtonClicked);
	NextButton->OnClicked().AddUObject(this, &UCharacterSelectWidget::OnNextButtonClicked);
	ConfirmButton->OnClicked().AddUObject(this, &UCharacterSelectWidget::OnConfirmButtonClicked);
}

EPlayerClass UCharacterSelectWidget::GetCurrentPlayerClass() const
{
	return CharacterDisplayInfo[CharacterInfoCarousel->GetActiveWidgetIndex()].GetPlayerClass();
}

void UCharacterSelectWidget::UpdateButtons()
{
	const FCharacterDisplayInfo& CurrentInfo = CharacterDisplayInfo[CharacterInfoCarousel->GetActiveWidgetIndex()];

	PreviousButton->SetIsEnabled(CurrentInfo.CanSelectPrevious());
	NextButton->SetIsEnabled(CurrentInfo.CanSelectNext());
}

void UCharacterSelectWidget::OnPreviousButtonClicked()
{
	const EPlayerClass PreviousPlayerClass = GetCurrentPlayerClass();

	CharacterInfoCarousel->NextPage();

	UpdateButtons();

	OnSelectedCharacterChangedDelegate.ExecuteIfBound(PreviousPlayerClass, GetCurrentPlayerClass());
}

void UCharacterSelectWidget::OnNextButtonClicked()
{
	const EPlayerClass PreviousPlayerClass = GetCurrentPlayerClass();

	CharacterInfoCarousel->PreviousPage();

	UpdateButtons();

	OnSelectedCharacterChangedDelegate.ExecuteIfBound(PreviousPlayerClass, GetCurrentPlayerClass());
}

void UCharacterSelectWidget::OnConfirmButtonClicked()
{
	OnCharacterChosenDelegate.ExecuteIfBound(GetCurrentPlayerClass());
}
