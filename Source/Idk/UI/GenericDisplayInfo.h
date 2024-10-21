// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/SoftObjectPtr.h>

#include "GenericDisplayInfo.generated.h"

class FString;
class UPaperSprite;
class UAttributeSystemComponent;
struct FLinearColor;

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateDescriptionDelegate, const FText&);
DECLARE_DELEGATE_OneParam(FGetDescriptionFormatArgsDelegate, FFormatNamedArguments&);

/** Contains information used to display anything with an icon, name, and description. */
USTRUCT()
struct FGenericDisplayInfo final
{
	GENERATED_BODY()

public:
	FGenericDisplayInfo() = default;

	/**
	 * @param DisplayName	Name to display. 
	 * @param Description	Description to display. 
	 * @param Icon			Icon to display.
	 */
	FGenericDisplayInfo(FText&& DisplayName, FText&& Description, UPaperSprite* Icon = nullptr);

	/**
	 * @param DisplayName	Name to display. 
	 * @param Description	Description to display.
	 * @param Icon			Icon to display.
	 */
	FGenericDisplayInfo(const FText& DisplayName, const FText& Description, UPaperSprite* Icon = nullptr);

	/**
	 * @param DisplayName	Name to display. 
	 * @param Description	Description to display. 
	 * @param Icon			Icon to display. 
	 */
	FGenericDisplayInfo(FString&& DisplayName, FString&& Description, UPaperSprite* Icon = nullptr);

	/** Initialize the description based on modifiers from the specified attribute system. */
	void InitDescription(UAttributeSystemComponent& AttributeSystem) const;

	/** Initialize the border color. */
	void InitBorderColor(const FLinearColor& InBorderColor);

	/** Get the name to display. */
	UE_NODISCARD const FText& GetDisplayName() const;

	/** Get the description to display. */
	UE_NODISCARD const FText& GetDescription() const;

	/** Get the icon to display. */
	UE_NODISCARD UPaperSprite* GetIcon() const;

	/** Check if the border color is set. */
	UE_NODISCARD bool IsBorderColorSet() const;

	/** Get the border color. */
	UE_NODISCARD const FLinearColor& GetBorderColor() const;

	/**
	 * Check if the display info is valid.
	 * 
	 * @param OutInvalidReason	Reason the display info was invalid. 
	 * @return					True if the display info is valid, otherwise false.
	 */
	UE_NODISCARD bool IsValid(FString& OutInvalidReason) const;

	/**
	 * Delegate called when the description changes. Used to update widgets.
	 * 
	 * @param Description Description after being formatted.
	 */
	mutable FUpdateDescriptionDelegate UpdateDescriptionDelegate;

	/**
	 * Delegate called to get the arguments used to format the description.
	 * 
	 * @param OutFormatArgs	Arguments used to format the description.
	 */
	FGetDescriptionFormatArgsDelegate GetDescriptionFormatArgsDelegate;

private:
	/** Update the formatted description. */
	void UpdateFormattedDescription() const;

	/** Check if the description can be formatted. */
	bool CanDescriptionBeFormatted() const;

	/** Name to display. */
	UPROPERTY(EditAnywhere)
	FText DisplayName;

	/** Description to display. */
	UPROPERTY(EditAnywhere, meta = (MultiLine))
	FText Description;

	/** Description after being formatted. */
	UPROPERTY(VisibleInstanceOnly, meta = (MultiLine))
	mutable FText FormattedDescription;

	/** Icon to display. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPaperSprite> Icon;

	/** Color of the border to display. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "bBorderColorSet", EditConditionHides))
	FLinearColor BorderColor = FLinearColor::Transparent;

	/** Whether the border color was set. */
	UPROPERTY()
	bool bBorderColorSet = false;
	
};
