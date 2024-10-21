// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GenericDisplayInfo.h"

#include "Idk/Character/Components/AttributeSystemComponent.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/Color.h>
#include <Misc/AssertionMacros.h>

FGenericDisplayInfo::FGenericDisplayInfo(FText&& DisplayName, FText&& Description, UPaperSprite* Icon)
	: DisplayName(DisplayName), Description(Description), FormattedDescription(Description), Icon(Icon)
{
}

FGenericDisplayInfo::FGenericDisplayInfo(const FText& DisplayName, const FText& Description, UPaperSprite* Icon)
	: DisplayName(DisplayName), Description(Description), FormattedDescription(Description), Icon(Icon)
{
}

FGenericDisplayInfo::FGenericDisplayInfo(FString&& DisplayName, FString&& Description, UPaperSprite* Icon)
	: DisplayName(FText::FromString(DisplayName)), Description(FText::FromString(Description)), FormattedDescription(FText::FromString(Description)), Icon(Icon)
{
}

void FGenericDisplayInfo::InitDescription(UAttributeSystemComponent& AttributeSystem) const
{
	if (CanDescriptionBeFormatted())
	{
		UpdateFormattedDescription();

		AttributeSystem.OnModifiersChangedDelegate.AddRaw(this, &FGenericDisplayInfo::UpdateFormattedDescription);
	}
	else if (!FormattedDescription.EqualTo(Description))
	{
		FormattedDescription = Description;
	}
}

void FGenericDisplayInfo::InitBorderColor(const FLinearColor& InBorderColor)
{
	bBorderColorSet = true;

	BorderColor = InBorderColor;
}

const FText& FGenericDisplayInfo::GetDisplayName() const
{
	return DisplayName;
}

const FText& FGenericDisplayInfo::GetDescription() const
{
	return FormattedDescription;
}

UPaperSprite* FGenericDisplayInfo::GetIcon() const
{
	return Icon.LoadSynchronous();
}

bool FGenericDisplayInfo::IsBorderColorSet() const
{
	return bBorderColorSet;
}

const FLinearColor& FGenericDisplayInfo::GetBorderColor() const
{
	ensure(bBorderColorSet);

	return BorderColor;
}

bool FGenericDisplayInfo::IsValid(FString& OutInvalidReason) const
{
	if (DisplayName.IsEmpty())
	{
		OutInvalidReason = TEXT("Display name is not set");

		return false;
	}

	if (Description.IsEmpty())
	{
		OutInvalidReason = TEXT("Description is not set");

		return false;
	}

	return true;
}

void FGenericDisplayInfo::UpdateFormattedDescription() const
{
	const FText PreviousDescription = FormattedDescription;

	TArray<FString> Params;

	FText::GetFormatPatternParameters(Description, Params);

	check(!Params.IsEmpty());

	FFormatNamedArguments Args;

	for (const FString& Param : Params)
	{
		Args.Emplace(Param);
	}

	GetDescriptionFormatArgsDelegate.Execute(Args);

	FormattedDescription = FText::Format(Description, Args);

	if (!FormattedDescription.EqualTo(PreviousDescription))
	{
		UpdateDescriptionDelegate.Broadcast(FormattedDescription);
	}
}

bool FGenericDisplayInfo::CanDescriptionBeFormatted() const
{
	TArray<FString> Params;

	FText::GetFormatPatternParameters(Description, Params);

	return Params.Num() > 0;
}
