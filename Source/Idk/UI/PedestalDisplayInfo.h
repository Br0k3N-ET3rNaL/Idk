// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>

struct FGenericDisplayInfo;

/** Information used to display items on pedestals. */
struct FPedestalDisplayInfo final
{
public:
	FPedestalDisplayInfo() = delete;

	/**
	 * @param DisplayInfo	Information that determines what to display.
	 * @param Price			Price of the item to display. Negative values will hide to price.
	 */
	FPedestalDisplayInfo(const FGenericDisplayInfo& DisplayInfo, const int32 Price = -1);

	/** Get the information that determines what to display. */
	UE_NODISCARD const FGenericDisplayInfo& GetDisplayInfo() const;

	/** Check if the price is set. */
	UE_NODISCARD bool HasPrice() const;

	/** Get the price to display. */
	UE_NODISCARD int32 GetPrice() const;

private:
	/** Information that determines what to display. */
	const FGenericDisplayInfo& DisplayInfo;

	/** Price of the item to display. Negative values will hide to price. */
	const int32 Price;
};

