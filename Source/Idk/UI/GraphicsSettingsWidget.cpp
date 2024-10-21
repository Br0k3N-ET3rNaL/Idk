// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/GraphicsSettingsWidget.h"

#include "Idk/UI/SettingsSubMenuWidget.h"
#include <Components/ComboBoxString.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <GameFramework/GameUserSettings.h>
#include <GenericPlatform/GenericWindow.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CString.h>
#include <Scalability.h>

const FString FScreenResolution::Delimiter = TEXT("x");

FScreenResolution::FScreenResolution(const int32 Width, const int32 Height)
	: Width(Width), Height(Height)
{
	String = FString::Printf(TEXT("%i%s%i"), Width, *Delimiter, Height);
}

FScreenResolution::FScreenResolution(const FIntPoint& IntPoint)
	: FScreenResolution(IntPoint.X, IntPoint.Y)
{
}

FScreenResolution::operator FIntPoint() const
{
	return FIntPoint(Width, Height);
}

bool FScreenResolution::operator==(const FScreenResolution& Other) const
{
	return Width == Other.Width && Height == Other.Height;
}

bool FScreenResolution::operator!=(const FScreenResolution& Other) const
{
	return !(*this == Other);
}

bool FScreenResolution::operator<(const FScreenResolution& Other) const
{
	return Width < Other.Width || (Width == Other.Width && Height < Other.Height);
}

int32 FScreenResolution::GetWidth() const
{
	return Width;
}

int32 FScreenResolution::GetHeight() const
{
	return Height;
}

const FString& FScreenResolution::ToString() const
{
	return String;
}

FScreenResolution FScreenResolution::FromString(const FString& String)
{
	FString WidthString;
	FString HeightString;

	String.Split(Delimiter, &WidthString, &HeightString);

	return FScreenResolution(FCString::Atoi(*WidthString), FCString::Atoi(*HeightString));
}

UGraphicsSettingsWidget::UGraphicsSettingsWidget()
{
	Resolutions.Add(FScreenResolution(1280, 720));
	Resolutions.Add(FScreenResolution(1920, 1080));
	Resolutions.Add(FScreenResolution(2560, 1440));
}

void UGraphicsSettingsWidget::ApplySettings()
{
	FString SelectedOption = ResolutionSelector->GetSelectedOption();

	const FScreenResolution SelectedResolution = FScreenResolution::FromString(SelectedOption);

	UserSettings->SetScreenResolution(SelectedResolution);

	const EWindowMode::Type WindowMode = WindowModeStringMap[WindowModeSelector->GetSelectedOption()];

	UserSettings->SetFullscreenMode(WindowMode);

	const int32 QualityPreset = QualitySelector->GetSelectedIndex();

	UserSettings->SetOverallScalabilityLevel(QualityPreset);

	UserSettings->ApplySettings(true);
}

void UGraphicsSettingsWidget::ResetToDefaults()
{
	UserSettings->SetToDefaults();

	UserSettings->SetScreenResolution(UserSettings->GetDesktopResolution());
	UserSettings->ApplySettings(true);

	InitValuesFromUserSettings();
}

void UGraphicsSettingsWidget::RevertToSavedValues()
{
	InitValuesFromUserSettings();
}

void UGraphicsSettingsWidget::Init(UGameUserSettings& InUserSettings)
{
	UserSettings = &InUserSettings;

	const FString Fullscreen = LexToString(EWindowMode::Type::Fullscreen);
	const FString WindowedFullscreen = LexToString(EWindowMode::Type::WindowedFullscreen);
	const FString Windowed = LexToString(EWindowMode::Type::Windowed);

	WindowModeStringMap.Add(Fullscreen, EWindowMode::Type::Fullscreen);
	WindowModeStringMap.Add(WindowedFullscreen, EWindowMode::Type::WindowedFullscreen);
	WindowModeStringMap.Add(Windowed, EWindowMode::Type::Windowed);

	WindowModeSelector->AddOption(Fullscreen);
	WindowModeSelector->AddOption(WindowedFullscreen);
	WindowModeSelector->AddOption(Windowed);

	for (int32 i = 0; i < NumQualityPresets; ++i)
	{
		QualitySelector->AddOption(Scalability::GetScalabilityNameFromQualityLevel(i).ToString());
	}

	AddResolutionOptions();

	InitValuesFromUserSettings();
}

void UGraphicsSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AddResolutionOptions();
}

void UGraphicsSettingsWidget::AddResolutionOptions()
{
	if (ResolutionSelector)
	{
		FString PrevSelectedOption = ResolutionSelector->GetSelectedOption();

		ResolutionSelector->ClearOptions();

		Resolutions.Sort([](const FScreenResolution& Lhs, const FScreenResolution& Rhs)
			{
				return Lhs < Rhs;
			});

		for (const FScreenResolution& Resolution : Resolutions)
		{
			ResolutionSelector->AddOption(Resolution.ToString());
		}

		ResolutionSelector->SetSelectedOption(PrevSelectedOption);
		ResolutionSelector->RefreshOptions();
	}
}

void UGraphicsSettingsWidget::InitValuesFromUserSettings()
{
	check(UserSettings);

	FIntPoint SavedResolution = UserSettings->GetScreenResolution();

	if (SavedResolution.X == 0 || SavedResolution.Y == 0)
	{
		SavedResolution = UserSettings->GetDesktopResolution();
	}

	const FScreenResolution& Resolution = Resolutions.FindOrAdd(SavedResolution);

	ResolutionSelector->SetSelectedOption(Resolution.ToString());
	ResolutionSelector->RefreshOptions();

	WindowModeSelector->SetSelectedOption(LexToString(UserSettings->GetFullscreenMode()));
	WindowModeSelector->RefreshOptions();

	QualitySelector->SetSelectedIndex(UserSettings->GetOverallScalabilityLevel());
	QualitySelector->RefreshOptions();
}
