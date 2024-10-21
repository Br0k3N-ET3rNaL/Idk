// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/InputSettingsWidget.h"

#include "Idk/Player/IdkPlayerController.h"
#include "Idk/UI/InputBindingRowWidget.h"
#include <UserSettings/EnhancedInputUserSettings.h>
#include <Blueprint/UserWidget.h>
#include <Components/ScrollBox.h>
#include <Containers/Map.h>
#include <CoreGlobals.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Logging/LogMacros.h>
#include <Misc/AssertionMacros.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>

void UInputSettingsWidget::ApplySettings()
{
	const int32 NumChildren = ScrollBox->GetChildrenCount();

	FMapPlayerKeyArgs MapArgs;

	MapArgs.ProfileId = InputSettings->GetCurrentKeyProfileIdentifier();
	MapArgs.Slot = EPlayerMappableKeySlot::First;

	for (int32 i = 0; i < NumChildren; ++i)
	{
		UInputBindingRowWidget* InputBindingRowWidget = CastChecked<UInputBindingRowWidget>(ScrollBox->GetChildAt(i));

		if (InputBindingRowWidget->HasChanged())
		{
			MapArgs.NewKey = InputBindingRowWidget->GetSelectedKey();
			MapArgs.MappingName = InputBindingRowWidget->GetMappingName();

			FGameplayTagContainer FailureReason;

			InputSettings->MapPlayerKey(MapArgs, FailureReason);

			if (!FailureReason.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("Input binding failed: %s"), *FailureReason.ToStringSimple());
			}

			InputBindingRowWidget->ApplyChanges();
		}
	}

	InputSettings->SaveSettings();
	InputSettings->ApplySettings();
}

void UInputSettingsWidget::ResetToDefaults()
{
	InputSettings->GetCurrentKeyProfile()->ResetToDefault();

	LoadSavedBindings();

	InputSettings->SaveSettings();
	InputSettings->ApplySettings();
}

void UInputSettingsWidget::RevertToSavedValues()
{
	LoadSavedBindings();
}

void UInputSettingsWidget::Init()
{
	AIdkPlayerController* PlayerController = CastChecked<AIdkPlayerController>(GetOwningPlayer());

	InputSettings = &PlayerController->GetInputSettings();

	LoadSavedBindings(true);
}

void UInputSettingsWidget::LoadSavedBindings(const bool bCreateRows)
{
	const TMap<FName, FKeyMappingRow>& MappedKeys = InputSettings->GetCurrentKeyProfile()->GetPlayerMappingRows();

	int32 Row = 0;

	for (const TPair<FName, FKeyMappingRow>& Pair : MappedKeys)
	{
		UInputBindingRowWidget* InputBindingRowWidget = (bCreateRows)
			? CreateWidget<UInputBindingRowWidget>(this, InputBindingRowClass.LoadSynchronous())
			: CastChecked<UInputBindingRowWidget>(ScrollBox->GetChildAt(Row));

		check(Pair.Value.Mappings.Num() == 1);

		for (const FPlayerKeyMapping& KeyMapping : Pair.Value.Mappings)
		{
			InputBindingRowWidget->Init(KeyMapping.GetMappingName(), KeyMapping.GetDisplayName(), KeyMapping.GetCurrentKey());

			break;
		}

		if (bCreateRows)
		{
			ScrollBox->AddChild(InputBindingRowWidget);
		}

		++Row;
	}
}
