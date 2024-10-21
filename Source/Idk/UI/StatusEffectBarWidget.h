// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "StatusEffectBarWidget.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UStatusEffectWidget;
class UWrapBox;

/** Widget that displays all status effects on a character. */
UCLASS(Abstract)
class UStatusEffectBarWidget final : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/**
	 * Add a new status effect widget.
	 * 
	 * @return	The newly created status effect widget. 
	 */
	UE_NODISCARD UStatusEffectWidget& AddStatusEffect();

	/** Remove a status effect widget. */
	void RemoveStatusEffect(UStatusEffectWidget& StatusEffect);

	/** Remove all status effect widgets. */
	void ClearStatusEffects();

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Wrap box containing status effect widgets. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox;

	/** Class of the widget used to display individual status effects. */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UStatusEffectWidget> StatusEffectWidgetClass;
};
