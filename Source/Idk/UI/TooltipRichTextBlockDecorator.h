// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/RichTextBlockDecorator.h>
#include <Containers/UnrealString.h>
#include <Framework/Text/ITextDecorator.h>
#include <Templates/SharedPointer.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>
#include <Widgets/SWidget.h>

#include "TooltipRichTextBlockDecorator.generated.h"

class FObjectInitializer;
class UKeywordTable;
class URichTextBlock;
class URichTextDecoratorTooltipWidget;
class UStatusEffectTable;
struct FTextBlockStyle;
struct FTextRunInfo;
struct FTextRunParseResults;

/** Rich text decorator used to embed tooltips within a rich text block. @see UTooltipRichTextBlockDecorator */
class FTooltipRichTextDecorator final : public FRichTextDecorator
{
public:
	FTooltipRichTextDecorator(URichTextBlock* InOwner, TSubclassOf<URichTextDecoratorTooltipWidget> RichTextDecoratorTooltipClass, UKeywordTable* KeywordTable, UStatusEffectTable* StatusEffectTable);

	//~ Begin FRichTextDecorator Interface.
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const override;
	//~ End FRichTextDecorator Interface

private:
	/** Class of the widget used to display the tooltip. @see URichTextDecoratorTooltipWidget */
	TSubclassOf<URichTextDecoratorTooltipWidget> RichTextDecoratorTooltipClass;

	/** Contains keywords and their descriptions. @see UKeywordTable */
	TObjectPtr<UKeywordTable> KeywordTable;

	/** Contains status effects and their descriptions. @see UStatusEffectTable */
	TObjectPtr<UStatusEffectTable> StatusEffectTable;

	/** Text used to indicate a keyword. */
	static const FString KeywordText;

	/** Text used to indicate an ID for a keyword or status effect. */
	static const FString IdText;

	/** Text used to indicate a status effect. */
	static const FString StatusEffectText;

	/** Text used to indicate a description for a status effect. */
	static const FString StatusEffectDescriptionText;
};

/** Rich text decorator used to embed tooltips within a rich text block. */
UCLASS(Abstract, Blueprintable)
class UTooltipRichTextBlockDecorator final : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UTooltipRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer);

	//~ Begin URichTextBlockDecorator Interface.
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
	//~ End URichTextBlockDecorator Interface

private:
	/** Class of the widget used to display the tooltip. @see URichTextDecoratorTooltipWidget */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<URichTextDecoratorTooltipWidget> RichTextDecoratorTooltipClass;

	/** Contains keywords and their descriptions. @see UKeywordTable */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UKeywordTable> KeywordTable;

	/** Contains status effects and their descriptions. @see UStatusEffectTable */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UStatusEffectTable> StatusEffectTable;
	
};
