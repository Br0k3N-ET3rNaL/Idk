// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/InputBindingRowWidget.h"

#include <Blueprint/UserWidget.h>
#include <Components/InputKeySelector.h>
#include <Components/TextBlock.h>
#include <InputCoreTypes.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Styling/SlateTypes.h>
#include <UObject/NameTypes.h>

void UInputBindingRowWidget::Init(const FName& InMappingName, const FText& DisplayName, const FKey& BoundInput)
{
	InputNameText->SetText(DisplayName);
	InputKeySelector->SetSelectedKey(BoundInput);

	SavedKey = InputKeySelector->GetSelectedKey().Key;
	MappingName = InMappingName;
}

bool UInputBindingRowWidget::HasChanged() const
{
	return SavedKey != InputKeySelector->GetSelectedKey().Key;
}

FKey UInputBindingRowWidget::GetSelectedKey() const
{
	return InputKeySelector->GetSelectedKey().Key;
}

const FName& UInputBindingRowWidget::GetMappingName() const
{
	return MappingName;
}

void UInputBindingRowWidget::ApplyChanges()
{
	SavedKey = InputKeySelector->GetSelectedKey().Key;
}

void UInputBindingRowWidget::RevertChanges()
{
	if (HasChanged())
	{
		InputKeySelector->SetSelectedKey(SavedKey);
	}
}

void UInputBindingRowWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FTextBlockStyle TextStyle;

	check(InputNameText);

	// Sync text style from the input name text to the input key selector

	TextStyle.SetFont(InputNameText->GetFont());
	TextStyle.SetColorAndOpacity(InputNameText->GetColorAndOpacity());
	TextStyle.SetShadowOffset(InputNameText->GetShadowOffset());
	TextStyle.SetShadowColorAndOpacity(InputNameText->GetShadowColorAndOpacity());

	InputKeySelector->SetTextStyle(TextStyle); 
	InputKeySelector->SetAllowModifierKeys(false);
}
