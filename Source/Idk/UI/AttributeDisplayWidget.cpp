// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/AttributeDisplayWidget.h"

#include "Idk/GameSystems/IdkAttribute.h"
#include <Blueprint/WidgetTree.h>
#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/Class.h>

const FTextFormat UAttributeDisplayWidget::ParenWrapFormatText = FText::FromString(TEXT("({0})"));
const FText UAttributeDisplayWidget::NoBaseValue = FText::FromString(TEXT("X"));
const FTextFormat UAttributeDisplayWidget::AdditionFormatText = FText::FromString(TEXT(" + {0}"));
const FTextFormat UAttributeDisplayWidget::SubtractionFormatText = FText::FromString(TEXT(" - {0}"));
const FTextFormat UAttributeDisplayWidget::BaseAndBonusFormatText = FText::FromString(TEXT("{0}{1}"));
const FTextFormat UAttributeDisplayWidget::MultiplierFormatText = FText::FromString(TEXT(" * {0}"));
const FTextFormat UAttributeDisplayWidget::FinalValueFormatText = FText::FromString(TEXT("{0} = {1}"));
const FTextFormat UAttributeDisplayWidget::FullFormatText = FText::FromString(TEXT("{0}{1}"));

void UAttributeDisplayWidget::Init(const TArray<FAttribute>& Attributes)
{
	for (const FAttribute& Attribute : Attributes)
	{
		UTextBlock* AttributeText = WidgetTree->ConstructWidget<UTextBlock>();

		AttributeText->SetText(UEnum::GetDisplayValueAsText(Attribute.GetAttributeType()));
		NameScrollBox->AddChild(AttributeText);

		UTextBlock* FormulaText = WidgetTree->ConstructWidget<UTextBlock>();

		FormulaText->SetText(GetFormula(Attribute));
		AttributeFormulas.Add(Attribute.GetAttributeType(), FormulaText);
		FormulaScrollBox->AddChild(FormulaText);
	}
}

void UAttributeDisplayWidget::Update(const FAttribute& Attribute)
{
	AttributeFormulas[Attribute.GetAttributeType()]->SetText(GetFormula(Attribute));
}

FText UAttributeDisplayWidget::GetFormula(const FAttribute& Attribute)
{
	const bool bHasBaseValue = Attribute.CanHaveBaseValue();
	const bool bHasBonus = Attribute.CanHaveBonus() && Attribute.GetBonus() != 0.0;
	const bool bHasMultiplierBonus = Attribute.CanHaveMultiplierBonus() && Attribute.GetMultiplierBonus() != 0.0;
	const bool bNegativeBonus = Attribute.GetBonus() < 0.0;

	// Text representing the base value of the attribute
	const FText& BaseValue = (bHasBaseValue) ? FText::AsNumber(Attribute.GetBaseValue()) : NoBaseValue;

	// Formula to plug the bonus into
	const FTextFormat& BonusFormatText = (bNegativeBonus) ? SubtractionFormatText : AdditionFormatText;

	// Text representing the bonus
	const FText& Bonus = (bHasBonus) ? FText::FormatOrdered(BonusFormatText, FText::AsNumber(Attribute.GetBonus())) : FText::GetEmpty();

	// Text representing the combination of the base value and the bonus
	FText BaseAndBonus = FText::FormatOrdered(BaseAndBonusFormatText, BaseValue, Bonus);

	// Wrap with parentheses if needed
	BaseAndBonus = (bHasBonus && bHasMultiplierBonus) ? FText::FormatOrdered(ParenWrapFormatText, BaseAndBonus) : BaseAndBonus;

	// Text representing the multiplier
	const FText& Multiplier = (bHasMultiplierBonus) ? FText::FormatOrdered(MultiplierFormatText, FText::AsNumber(1.0 + Attribute.GetMultiplierBonus())) : FText::GetEmpty();

	// Combine the base value and bonus with the multiplier 
	FText FinalValue = FText::FormatOrdered(FullFormatText, BaseAndBonus, Multiplier);

	FinalValue = (bHasBaseValue && (bHasBonus || bHasMultiplierBonus))
		? FText::FormatOrdered(FinalValueFormatText, FinalValue, FText::AsNumber(Attribute.GetFinalValue()))
		: FinalValue;

	return  FinalValue;
}
