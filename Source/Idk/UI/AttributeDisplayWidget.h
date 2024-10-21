// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AttributeDisplayWidget.generated.h"

enum class EAttributeType : uint8;
class UScrollBox;
class USingleAttributeDisplayWidget;
class UTextBlock;
struct FAttribute;

/** Widget used to display the formula for each attribute's current value. */
UCLASS(Abstract)
class UAttributeDisplayWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the attribute display widget.
	 * 
	 * @param Attributes	Attributes to display. 
	 */
	void Init(const TArray<FAttribute>& Attributes);

	/**
	 * Update the display for a single attribute.
	 * 
	 * @param Attribute	Attribute to update, with updated values. 
	 */
	void Update(const FAttribute& Attribute);

private:
	/** Get the formula for the specified attribute's current value. */
	UE_NODISCARD static FText GetFormula(const FAttribute& Attribute);

	/** Scroll box to add the names of attributes to. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> NameScrollBox;

	/** Scroll box to add the attributes' displays to. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> FormulaScrollBox;

	/** Maps attribute types to the text block used to display the formula for that attribute type. */
	TMap<EAttributeType, UTextBlock*> AttributeFormulas;

	/** Text used to represent the base value for an attribute without one. */
	static const FText NoBaseValue;

	/** Format text used to wrap text with parentheses. */
	static const FTextFormat ParenWrapFormatText;

	/** Format text used for positive bonuses. */
	static const FTextFormat AdditionFormatText;

	/** Format text used for negative bonuses. */
	static const FTextFormat SubtractionFormatText;

	/** Format text used to combine the base value with the bonus value. */
	static const FTextFormat BaseAndBonusFormatText;

	/** Format text used for the multiplier. */
	static const FTextFormat MultiplierFormatText;

	/** Format text used to combine the full formula with the final value. */
	static const FTextFormat FinalValueFormatText;

	/** Format text used to combine the base value and bonus with the multiplier. */
	static const FTextFormat FullFormatText;

};
