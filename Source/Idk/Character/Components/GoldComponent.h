// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "GoldComponent.generated.h"

class UGoldWidget;

/** Component that manages gold. */
UCLASS( ClassGroup=(Custom), NotBlueprintable, meta=(BlueprintSpawnableComponent) )
class UGoldComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGoldComponent();

	/** Set the current amount of gold. */
	void SetGold(const int32 InGold);

	/** Add Gold. */
	void AddGold(const int32 GoldToAdd);

	/** Remove gold. */
	void RemoveGold(const int32 GoldToLose);

	/** Get the current amount of gold. */
	UE_NODISCARD int32 GetGold() const;

	/** Set the widget used to display the amount of gold. */
	void SetGoldWidget(UGoldWidget& InGoldWidget);

private:
	/** Update the widget based on current amount of gold. */
	void UpdateWidget();

	/** Current amount of gold. */
	UPROPERTY(VisibleInstanceOnly)
	int32 Gold = 300;

	/** Widget used to display the amount of gold. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UGoldWidget> GoldWidget;

	/** Maximum amount of gold. */
	static const int32 MaxGold = 999999999;
	
};
