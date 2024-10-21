// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/TooltipRichTextBlockDecorator.h"

#include "Idk/EffectSystem/StatusEffectTable.h"
#include "Idk/UI/KeywordTable.h"
#include "Idk/UI/RichTextDecoratorTooltipWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/RichTextBlock.h>
#include <Components/RichTextBlockDecorator.h>
#include <Containers/UnrealString.h>
#include <CoreGlobals.h>
#include <Framework/Text/ITextDecorator.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Logging/LogMacros.h>
#include <Misc/Optional.h>
#include <Styling/SlateTypes.h>
#include <Templates/SharedPointer.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UObjectGlobals.h>
#include <Widgets/SWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(TooltipRichTextBlockDecorator)

const FString FTooltipRichTextDecorator::KeywordText = TEXT("Keyword");
const FString FTooltipRichTextDecorator::IdText = TEXT("id");
const FString FTooltipRichTextDecorator::StatusEffectText = TEXT("StatusEffect");
const FString FTooltipRichTextDecorator::StatusEffectDescriptionText = TEXT("Desc");

FTooltipRichTextDecorator::FTooltipRichTextDecorator(URichTextBlock* InOwner, TSubclassOf<URichTextDecoratorTooltipWidget> RichTextDecoratorTooltipClass, UKeywordTable* KeywordTable, UStatusEffectTable* StatusEffectTable)
	: FRichTextDecorator(InOwner), RichTextDecoratorTooltipClass(RichTextDecoratorTooltipClass), KeywordTable(KeywordTable), StatusEffectTable(StatusEffectTable)
{
}

bool FTooltipRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	/**
	 * Valid formats:
	 * 
	 * <Keyword id="Keyword ID here">
	 * <StatusEffect id="Status effect ID here">
	 * <StatusEffect Desc="Status effect description here">
	 */
	return KeywordTable && StatusEffectTable
		&& (RunParseResult.Name.Equals(KeywordText) && RunParseResult.MetaData.Contains(IdText)
			|| (RunParseResult.Name.Equals(StatusEffectText)
				&& (RunParseResult.MetaData.Contains(IdText) || RunParseResult.MetaData.Contains(StatusEffectDescriptionText))));
}

TSharedPtr<SWidget> FTooltipRichTextDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const
{
	URichTextDecoratorTooltipWidget* DecoratorTooltipWidget = CreateWidget<URichTextDecoratorTooltipWidget>(Owner, RichTextDecoratorTooltipClass);

	FTextBlockStyle TextStyle;
	FText Description;

	if (FRichTextStyleRow* TextStyleRow = Owner->GetTextStyleSet()->FindRow<FRichTextStyleRow>(*KeywordText, {}))
	{
		TextStyle = TextStyleRow->TextStyle;
	}
	else
	{
		TextStyle = FTextBlockStyle(DefaultTextStyle);
		TextStyle.Font.TypefaceFontName = FName(TEXT("Bold"));
	}

	if (RunInfo.Name.Equals(KeywordText))
	{
		// Check if the keyword is valid
		if (const FKeywordInfo* KeywordInfo = KeywordTable->KeywordTable.Find(RunInfo.MetaData[IdText]))
		{
			TextStyle.SetColorAndOpacity(KeywordInfo->GetTextColor());
			Description = KeywordInfo->GetDescription();
		}
		else
		{
			Description = FText::FromString(TEXT("Keyword not found"));

			UE_LOG(LogTemp, Warning, TEXT("Keyword \"%s\" not found"), *RunInfo.MetaData[IdText]);
		}
	}
	else if (RunInfo.Name.Equals(StatusEffectText))
	{
		if (RunInfo.MetaData.Contains(IdText))
		{
			TOptional<FKeywordInfo> KeywordInfo = StatusEffectTable->GetKeywordInfo(RunInfo.MetaData[IdText]);

			if (KeywordInfo.IsSet())
			{
				TextStyle.SetColorAndOpacity(KeywordInfo->GetTextColor());
				Description = KeywordInfo->GetDescription();
			}
			else
			{
				Description = FText::FromString(TEXT("Status effect not found"));

				UE_LOG(LogTemp, Warning, TEXT("Status effect \"%s\" not found"), *RunInfo.MetaData[IdText]);
			}
		}
		else if (RunInfo.MetaData.Contains(StatusEffectDescriptionText))
		{
			Description = FText::FromString(RunInfo.MetaData[StatusEffectDescriptionText]);
		}
	}

	DecoratorTooltipWidget->Init(RunInfo.Content, Description, TextStyle);

	return DecoratorTooltipWidget->TakeWidget();
}

UTooltipRichTextBlockDecorator::UTooltipRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UTooltipRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	if (!RichTextDecoratorTooltipClass.IsNull())
	{
		return MakeShareable(new FTooltipRichTextDecorator(InOwner, RichTextDecoratorTooltipClass.LoadSynchronous(), KeywordTable.LoadSynchronous(), StatusEffectTable.LoadSynchronous()));
	}
	else
	{
		return nullptr;
	}
}
