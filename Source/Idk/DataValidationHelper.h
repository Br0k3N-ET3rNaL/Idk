// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Internationalization/Text.h>

class FDataValidationContext;

/** Contains functions used by data validation. */
struct FDataValidationHelper final
{
public:
	FDataValidationHelper() = delete;

	/**
	 * Append issues from one data validation to another, adding prefixes for each issue.
	 * 
	 * @param Prefix			Prefix to add to each issue before appending.			 
	 * @param Context			Data validation context to add issues to.
	 * @param ContextToAppend	Data validation context to append to the other data validation context.
	 */
	static void AddPrefixAndAppendIssues(const FText& Prefix, FDataValidationContext& Context, const FDataValidationContext& ContextToAppend);

private:
	/** Format text used to add a prefix to an issue. */
	static const FTextFormat PrefixFormatText;
};

