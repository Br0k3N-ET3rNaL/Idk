// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "TooltipMenuAnchorWidget.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FText;
class UMenuAnchor;
class UTooltipWidget;

/** Widget used to show/hide a tooltip. */
UCLASS(Abstract)
class UTooltipMenuAnchorWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Initialize the tooltip menu anchor widget.
	 * 
	 * @param Title			Title to display in the tooltip. 
	 * @param Description	Description to display in the tooltip.
	 */
	void Init(const FText& Title, const FText& Description);

	/** Display the tooltip. */
	void ShowTooltip();

	/** Hide the tooltip. */
	void HideTooltip();

	/** Gray out the description. */
	void DisableDescription();

	/** Get the widget used to display the tooltip. */
	UE_NODISCARD UTooltipWidget& GetTooltipWidget();

private:
	/** Get the tooltip widget for the menu anchor. */
	UFUNCTION()
	UUserWidget* GetTooltipWidgetInternal();

	/** Class of the widget used to display the tooltip. */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UTooltipWidget> CustomTooltipClass;

	/** Widget used to display the tooltip. @see UTooltipWidget */
	UPROPERTY()
	TObjectPtr<UTooltipWidget> CustomTooltipWidget;

	/** Menu anchor that is used to show/hide the tooltip. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuAnchor> MenuAnchor;
};
