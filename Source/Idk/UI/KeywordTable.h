// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/Color.h>
#include <UObject/ObjectMacros.h>

#include "KeywordTable.generated.h"

/** Information used to display a keyword within a tooltip. */
USTRUCT()
struct FKeywordInfo final
{
	GENERATED_BODY()

public:
	FKeywordInfo() = default;

	/**
	 * @param Description	Description of the keyword. 
	 * @param TextColor		Color of the keyword in tooltips.
	 */
	FKeywordInfo(const FString& Description, const FLinearColor& TextColor);

	/**
	 * @param Description	Description of the keyword.
	 * @param TextColor		Color of the keyword in tooltips.
	 */
	FKeywordInfo(const FText& Description, const FLinearColor& TextColor);

	/** Get the description of the keyword. */
	UE_NODISCARD const FText& GetDescription() const;

	/** Get the color of the keyword in tooltips. */
	UE_NODISCARD const FLinearColor& GetTextColor() const;

private:
	/** Description of the keyword. */
	UPROPERTY(EditDefaultsOnly, meta = (MultiLine))
	FText Description;

	/** Color of the keyword in tooltips. */
	UPROPERTY(EditDefaultsOnly)
	FLinearColor TextColor = FLinearColor::White;

};

/** Contains all possible keywords that can be used in tooltips. */
UCLASS()
class UKeywordTable final : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UKeywordTable();

	/** Maps keywords to information used to display that keyword in tooltips. @see FKeywordInfo */
	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TMap<FString, FKeywordInfo> KeywordTable;
};
