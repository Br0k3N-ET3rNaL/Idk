// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RemoveItemDelegate.h"
#include "Idk/GameSystems/RoomManagers/HallRoomManager.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "CurseRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class ARewardPedestalSelector;
class FDataValidationContext;
class UItem;
class USingleStageEffect;

/** Room that offers items a choice between items that have additional negative effects. */
UCLASS(Blueprintable)
class ACurseRoomManager final : public AHallRoomManager
{
	GENERATED_BODY()
	
public:
	ACurseRoomManager();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

private:
	//~ Begin ARoomManager Interface.
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	//~ End ARoomManager Interface

	/**
	 * Event called when a pedestal is selected.
	 * 
	 * @param Index	Index of the selected pedestal.
	 */
	void OnPedestalSelected(const uint8 Index);

	/** Blueprint class that offers the player a choice between items. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftClassPtr<ARewardPedestalSelector> PedestalSelectorClass;

	/** Offers the player a choice between items. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ARewardPedestalSelector> PedestalSelector;

	/** Cursed items that have been generated. */
	UPROPERTY(VisibleInstanceOnly)
	TArray<UItem*> GeneratedItems;

	/** Delegates that can be called to remove generated items from the item pool. */
	TArray<FRemoveItemDelegate> RemoveItemDelegates;

	/** Display info for the option to skip the cursed items. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	FGenericDisplayInfo SkipCurseDisplayInfo;

	/** Effect applied to the player when they choose to skip the cursed items. */
	UPROPERTY(EditDefaultsOnly, Instanced, NoClear, Category = "Idk")
	TObjectPtr<USingleStageEffect> SkipCurseEffect;

	/** Index of the reward pedestal that provides the option to skip the cursed items. */
	static const uint8 SkipCursePedestalIndex = 0;
};
