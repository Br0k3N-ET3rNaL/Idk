// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <HAL/Platform.h>
#include <InputCoreTypes.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "InputBindingRowWidget.generated.h"

class FText;
class UInputKeySelector;
class UTextBlock;

/** Widget that represents an input action that can be bound to a key. */
UCLASS(Abstract)
class UInputBindingRowWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the input binding row widget.
	 * 
	 * @param InMappingName	Name of the mapping. 
	 * @param DisplayName	Name to display for the mapping.
	 * @param BoundInput	Key currently bound for the mapping. 
	 */
	void Init(const FName& InMappingName, const FText& DisplayName, const FKey& BoundInput);

	/** Check if the selected key has changed. */
	UE_NODISCARD bool HasChanged() const;

	/** Get the currently selected key. */
	UE_NODISCARD FKey GetSelectedKey() const;

	/** Get the name of the mapping. */
	UE_NODISCARD const FName& GetMappingName() const;

	/** Apply changes to the selected key. */
	void ApplyChanges();

	/** Revert changes to the selected key. */
	void RevertChanges();

private:
	//~ Begin UUserWidget Interface.
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	/** Text that displays the name of the mapping. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InputNameText;

	/** Widget that allows an input key to be selected. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	/** Name of the mapping. */
	FName MappingName;

	/** Key that was last saved. */
	FKey SavedKey;
	
};
