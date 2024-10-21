// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/GameSystems/RemoveItemDelegate.h"
#include "Idk/GameSystems/RoomManagers/RoomManager.h"
#include <Containers/Array.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "ArenaRoomManager.generated.h"

enum class EDataValidationResult : uint8;
class AArenaMeshManager;
class AIdkPlayerCharacter;
class ARandomEnemySpawner;
class ARewardPedestalSelector;
class FDataValidationContext;
class UChildActorComponent;
class UItem;
struct FRoomSpawnInfo;

DECLARE_DELEGATE_RetVal(FTransform, FGetArenaPortalDestDelegate);

/** Base room manager class for arenas. */
UCLASS(Abstract, NotBlueprintable)
class AArenaRoomManager : public ARoomManager
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface.
	virtual void Destroyed() override;
	//~ End AActor Interface

	/**
	 * @brief 
	 * @param RoomSpawnInfo 
	 * @param BoxExtents 
	 */
	virtual void Init(const FRoomSpawnInfo& RoomSpawnInfo, const FVector& BoxExtents);

	/**
	 * @brief 
	 * @param bInLeftPortal 
	 */
	void EnableSpawnPortal(const bool bInLeftPortal);

	/** Get the transform of a portal exiting this arena. */
	UE_NODISCARD FTransform GetPortalSource() const;

	/** 
	 * Get the transform of a portal entering this arena. 
	 * 
	 * @param bLeft	Whether to get the transform of the left or right side of the arena.
	 */
	UE_NODISCARD FTransform GetPortalDestination(const bool bLeft) const;

	/**
	 * Delegate that will get the transform of the exit portal connected to a portal leaving this arena.
	 *
	 * @return	Transform of the exit portal connected to a portal leaving this arena.
	 */
	FGetArenaPortalDestDelegate GetPortalDestDelegate;

protected:
	//~ Begin ARoomManager Interface.
	virtual void OnEnabled() override;
	virtual void OnDisabled() override;
	virtual void OnRoomActivated() override;
	virtual void OnCompletion() override;
	//~ End ARoomManager Interface

	/**
	 * Event called when a pedestal is selected.
	 * 
	 * @param Index Index of the selected pedestal.
	 */
	void OnPedestalSelected(const uint8 Index);

	/** Event that should be called when all spawned enemies have been killed. */
	virtual void OnAllEnemiesKilled();

	/** Blueprint class that offers the player a choice between items. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Required Bindings")
	TSoftClassPtr<ARewardPedestalSelector> RewardPedestalSelectorClass;

	/** Blueprint class that manages static meshes for the arena. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Required Bindings")
	TSoftClassPtr<AArenaMeshManager> MeshManagerClass;

	/** Manages static meshes for the arena.  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AArenaMeshManager> MeshManager;

	/** Offers the player a choice between items. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<ARewardPedestalSelector> RewardPedestalSelector;

	/** Generated items. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<UItem*> GeneratedItems;

	/** If a portal is spawned on completion, determines whether the portal should be spawned or left or right side. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bLeftPortal = false;

	/** Level used when generating items. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	uint8 RewardLevel = 1;

	/** Delegates that can be called to remove generated items from the item pool. */
	TArray<FRemoveItemDelegate> RemoveItemDelegates;

private:
	/** Spawn a portal connecting this arena to a hall. */
	void SpawnPortal();
};
