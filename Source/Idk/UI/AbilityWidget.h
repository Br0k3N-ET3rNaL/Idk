// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Internationalization/Text.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "AbilityWidget.generated.h"

class UGenericDisplayWidget;
class UProgressBar;
class UTextBlock;
class UWidgetAnimation;
struct FGenericDisplayInfo;

/** Widget that displays an ability and its cooldown. */
UCLASS(Abstract)
class UAbilityWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface.
	void NativeConstruct() override;
	//~ End UUserWidget Interface

	/**
	 * Initialize the ability widget.
	 * 
	 * @param DisplayInfo	Information used to display the ability. @see FGenericDisplayInfo 
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

	/**
	 * Update the cooldown of the ability.
	 * 
	 * @param SecondsRemaining	Seconds until the ability is off cooldown. 
	 * @param PercentRemaining	Percentage of cooldown remaining.			 
	 */
	void UpdateCooldown(const double SecondsRemaining, const double PercentRemaining);

	/** Grays out the ability, indicating that it can't be used. */
	void DisableAbility();

	/** Hides the cooldown and plays an animation to indicate that the cooldown is complete. */
	void EndCooldown();

private:
	/** Widget that displays the ability. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> DisplayWidget;

	/** Progress bar representing the current cooldown percentage. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CooldownPercent;

	/** Text used to display the cooldown remaining in seconds. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownRemaining;

	/** Widget animation to play when the cooldown is complete. */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CooldownDoneAnimation;

	/** Number formatting to use for the cooldown remaining. */
	FNumberFormattingOptions NumberFormatting;
};
