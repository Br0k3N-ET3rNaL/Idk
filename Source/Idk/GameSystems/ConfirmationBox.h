// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Delegates/DelegateCombinations.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "ConfirmationBox.generated.h"

enum class EDataValidationResult : uint8;
class AIdkPlayerCharacter;
class FDataValidationContext;
class FText;
class UWidgetComponent;
class UConfirmationWidget;

DECLARE_DELEGATE_OneParam(FOnOptionSelectedDelegate, bool);

/** Shows the player a message and allows them to either confirm or deny. */
UCLASS(Abstract)
class AConfirmationBox final : public AActor
{
	GENERATED_BODY()
	
public:	
	AConfirmationBox();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Set the message to display to the player. */
	void SetMessage(const FText& Message);

	/**
	 * Delegate called when an option is selected.
	 * 
	 * @param bConfirmed	Whether confirm or deny was selected.
	 */
	FOnOptionSelectedDelegate OnOptionSelectedDelegate;

private:
	/** Class of the widget used to display the message and options. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<UConfirmationWidget> ConfirmationWidgetClass;

	/** Size of the created widget. */
	UPROPERTY(EditDefaultsOnly)
	FVector2D DrawSize = {200.0, 100.0};

	/** How much to upscale the created widget. */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.0))
	double UpscaleFactor = 5.0;

	/** Widget used to display the message and options. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> ConfirmationWidgetComp;

};
