// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/PlayerClasses.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Math/Color.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "FullItemPool.generated.h"

enum class EDataValidationResult : uint8;
enum class EItemRarity : uint8;
class FDataValidationContext;
class UItem;
class UStatusEffectBlueprintBase;
class UStatusEffectTable;

/** An array of items. Used by UFullItemPool. */
USTRUCT()
struct FDefaultItemArray final
{
	GENERATED_BODY()

	friend struct FWeightedItemArray;
	friend struct FBackupItemArray;
	friend struct FDefaultItemGroup;
	friend class UFullItemPool;
	friend class UItemPool;

private:
#if WITH_EDITOR
	/** @see UObject::IsDataValid */
	UE_NODISCARD EDataValidationResult IsDataValid(FDataValidationContext& Context) const;
#endif

	/** Array of items. */
	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<UItem*> Items;
};

/** Group of item arrays for each item rarity. */
USTRUCT()
struct FDefaultItemGroup final
{
	GENERATED_BODY()

	friend struct FItemGroup;
	friend class UFullItemPool;
	friend class UItemPool;

public:
	FDefaultItemGroup();

private:
#if WITH_EDITOR
	/** @see UObject::IsDataValid */
	UE_NODISCARD EDataValidationResult IsDataValid(FDataValidationContext& Context) const;
#endif

	/** Maps item rarities to their corresponding item arrays. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EItemRarity, FDefaultItemArray> Items;

};

/** Contains all possible items. */
UCLASS()
class UFullItemPool final : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFullItemPool();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Colors used to display each rarity. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EItemRarity, FLinearColor> RarityColors;

	/** Items generated after all other items have been generated. Can be generated more than once. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EItemRarity, FDefaultItemArray> BackupItems;

	/** Items available to all player classes. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk")
	FDefaultItemGroup SharedItems;

	/** Items available to only one specific player class. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk", meta = (ForceInlineRow, ReadOnlyKeys))
	TMap<EPlayerClass, FDefaultItemGroup> ClassItems;

	/** Object containing all reusable status effects. */
	UPROPERTY()
	TObjectPtr<UStatusEffectTable> StatusEffectTable;

private:
	/**
	 * Functions that create items for each sub-group.
	 */

	void CreateBackupItems();
	void CreateCommonBackupItems();
	void CreateUncommonBackupItems();
	void CreateRareBackupItems();
	void CreateLegendaryBackupItems();
	void CreateCursedBackupItems();

	void CreateSharedItems();
	void CreateCommonSharedItems();
	void CreateUncommonSharedItems();
	void CreateRareSharedItems();
	void CreateLegendarySharedItems();
	void CreateCursedSharedItems();

	void CreateKnightItems();
	void CreateCommonKnightItems();
	void CreateUncommonKnightItems();
	void CreateRareKnightItems();
	void CreateLegendaryKnightItems();
	void CreateCursedKnightItems();

	void CreateMageItems();
	void CreateCommonMageItems();
	void CreateUncommonMageItems();
	void CreateRareMageItems();
	void CreateLegendaryMageItems();
	void CreateCursedMageItems();

	/** Gets the blueprint class of the status effect with the given name. */
	TSubclassOf<UStatusEffectBlueprintBase> LoadStatusEffectByName(FString&& StatusEffectName) const;

	/** Path to the status effect table data asset. */
	static const FString StatusEffectTablePath;

};
