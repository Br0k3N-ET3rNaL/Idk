// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "StatusEffectWidget.generated.h"

class UGenericDisplayWidget;
class UImage;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTextBlock;
struct FGenericDisplayInfo;

/** Widget that displays a status effect. @see UStatusEffectBarWidget, UStatusEffect */
UCLASS(Abstract)
class UStatusEffectWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the status effect widget.
	 * 
	 * @param DisplayInfo	Information used to display the status effect. @see FGenericDisplayInfo 
	 */
	void Init(const FGenericDisplayInfo& DisplayInfo);

	/** Update the percentage of time remaining for the status effect. */
	void UpdateTimeRemaining(const double DurationRemainingPercent);

	/** Update the current number of stacks for the status effect. */
	void UpdateStacks(const uint8 Stacks);

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override final;
	//~ End UUserWidget Interface

	/** Material used to display the percentage of time remaining for a status effect. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	TSoftObjectPtr<UMaterialInterface> RadialProgressMaterial;

	/** Instance of the material used to display the percentage of time remaining for the status effect. */
	UPROPERTY(VisibleInstanceOnly, Category = "Idk")
	TObjectPtr<UMaterialInstanceDynamic> RadialProgressMaterialInstance;

	/** Widget used to display the status effect's icon, name, and description. @see UGenericDisplayWidget */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericDisplayWidget> DisplayWidget;

	/** Widget that displays the percentage of time remaining for the status effect. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DurationRemainingOverlay;

	/** Text that displays the current number of stacks of the status effect. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StacksCounter;

	/** Name of the material parameter representing the fill percentage. */
	static const FName FillPercent;

};
