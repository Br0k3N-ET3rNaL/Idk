// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/DataValidationHelper.h"

#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/DataValidation.h>

const FTextFormat FDataValidationHelper::PrefixFormatText = FText::FromString(TEXT("{0}{1}"));

void FDataValidationHelper::AddPrefixAndAppendIssues(const FText& Prefix, FDataValidationContext& Context, const FDataValidationContext& ContextToAppend)
{
	TArray<FText> Warnings;
	TArray<FText> Errors;

	ContextToAppend.SplitIssues(Warnings, Errors);

	for (FText& Warning : Warnings)
	{
		Context.AddWarning(FText::FormatOrdered(PrefixFormatText, Prefix, Warning));
	}

	for (FText& Error : Errors)
	{
		Context.AddError(FText::FormatOrdered(PrefixFormatText, Prefix, Error));
	}
}
