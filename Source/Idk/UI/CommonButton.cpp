// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/CommonButton.h"

#include <CommonButtonBase.h>
#include <CommonTextBlock.h>
#include <Components/Border.h>
#include <Components/BorderSlot.h>
#include <Components/ScaleBox.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Layout/Margin.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <Types/SlateEnums.h>
#include <UObject/NameTypes.h>
#include <UObject/UnrealType.h>
#include <Widgets/Layout/SScaleBox.h>

void UCommonButton::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonPadding = FMargin(4.f, 2.f);
	HorizontalAlignment = EHorizontalAlignment::HAlign_Center;
	VerticalAlignment = EVerticalAlignment::VAlign_Center;
	Stretch = EStretch::None;
	Text = FText::FromString(TEXT("TextBlock"));
}

bool UCommonButton::Initialize()
{
	const bool bInitializedThisCall = Super::Initialize();

	if (bInitializedThisCall)
	{
		ButtonText->SetText(Text);

		UBorderSlot* BorderSlot = CastChecked<UBorderSlot>(Border->GetContentSlot());

		BorderSlot->SetPadding(ButtonPadding);
		BorderSlot->SetHorizontalAlignment(HorizontalAlignment);
		BorderSlot->SetVerticalAlignment(VerticalAlignment);

		ScaleBox->SetStretch(Stretch);
	}

	return bInitializedThisCall;
}

void UCommonButton::SetText(const FText& InText)
{
	Text = InText;

	ButtonText->SetText(Text);
}

#if WITH_EDITOR
void UCommonButton::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UCommonButton, Text)))
	{
		if (ButtonText)
		{
			ButtonText->SetText(Text);
		}
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UCommonButton, ButtonPadding)))
	{
		if (Border)
		{
			CastChecked<UBorderSlot>(Border->GetContentSlot())->SetPadding(ButtonPadding);
		}
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UCommonButton, HorizontalAlignment)))
	{
		if (Border)
		{
			CastChecked<UBorderSlot>(Border->GetContentSlot())->SetHorizontalAlignment(HorizontalAlignment);
		}
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UCommonButton, VerticalAlignment)))
	{
		if (Border)
		{
			CastChecked<UBorderSlot>(Border->GetContentSlot())->SetVerticalAlignment(VerticalAlignment);
		}
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UCommonButton, Stretch)))
	{
		if (ScaleBox)
		{
			ScaleBox->SetStretch(Stretch);
		}
	}
}
#endif

void UCommonButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	ButtonText->SetStyle(GetCurrentTextStyleClass());
}
