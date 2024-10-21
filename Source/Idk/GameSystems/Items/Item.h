// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/UI/GenericDisplayInfo.h"
#include <GameplayTagAssetInterface.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Misc/EnumRange.h>
#include <NativeGameplayTags.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "Item.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UAttributeSystemComponent;
class UItemEffect;
struct FItemSaveData;
struct FLinearColor;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Type1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Type2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Type2);

/** Enum representing item rarity. */
UENUM()
enum class EItemRarity : uint8
{
	None UMETA(Hidden),
	Common,
	Uncommon,
	Rare,
	Legendary,
	Cursed,
	Num UMETA(Hidden),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EItemRarity, EItemRarity::Common, EItemRarity::Cursed);

/** Represents an item that can be added to a character's inventory to grant them effects. */
UCLASS(EditInlineNew, CollapseCategories, NotBlueprintable)
class UItem final : public UObject, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	friend class FItemBuilder;
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	//~ Begin IGameplayTagAssetInterface Interface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface

	UE_NODISCARD bool operator<(const UItem& Other) const;
	UE_NODISCARD bool operator==(const UItem& Other) const;
	friend bool operator==(const UItem* Lhs, const UItem& Rhs);

	/** Get the name of the item. */
	UE_NODISCARD const FName& GetItemName() const;

	/** Get the information used to display the item to the player. */
	UE_NODISCARD const FGenericDisplayInfo& GetDisplayInfo() const;

	/** Get the rarity of the item. */
	UE_NODISCARD EItemRarity GetRarity() const;

	/** Get the number of stacks of the item. */
	UE_NODISCARD uint8 GetStacks() const;

	/** Check whether multiple stacks of the same item can be added to the same character. */
	UE_NODISCARD bool IsStackable() const;

	/** Check if the item is disabled. */
	UE_NODISCARD bool IsDisabled() const;

	/**
	 * Initialize the rarity and the color representing the rarity.
	 * 
	 * @param InRarity		Rarity of the item.
	 * @param RarityColor	Color used to display the rarity.
	 */
	void InitRarity(const EItemRarity InRarity, const FLinearColor& RarityColor);

	/** Set the index of the item. Used by save data. */
	void SetIndex(const int32 InIndex);

	/** Allow the item to stack. */
	void EnableStacking();

	/** Disable the item. */
	void DisableItem();

	/** Get the data needed to save this item. */
	UE_NODISCARD FItemSaveData GetItemSaveData() const;

	/** Check if the item matches the supplied save data. */
	UE_NODISCARD bool MatchesSaveData(const FItemSaveData& SaveData) const;
	
	/**
	 * Apply the item's effect to the target attribute system.
	 * 
	 * @param Target		Attribute system to apply effect to.
	 * @param bUpdateStacks If true, will update the number of stacks of the item.
	 */
	void ApplyEffect(UAttributeSystemComponent* Target, const bool bUpdateStacks = true) const;

	/**
	 * Remove the item's effect from the target attribute system.
	 * 
	 * @param Target			Attribute system to remove effect from.
	 * @param bRemoveAllStacks	Whether to remove all stacks of the item, or only one.
	 */
	void RemoveEffect(UAttributeSystemComponent* Target, const bool bRemoveAllStacks = true) const;

	/**
	 * Duplicate the supplied item.
	 * 
	 * @param Outer	Used as the outer object when creating the duplicate.
	 * @param Item	Item to duplicate.
	 * @return		A copy of the supplied item.
	 */
	UE_NODISCARD static UItem* DuplicateItem(UObject* Outer, const UItem& Item);

private:
	/** Name of the item. Should be unique */
	UPROPERTY(EditDefaultsOnly)
	FName Name;

	/** Information used to display the item to the player. */
	UPROPERTY(EditDefaultsOnly, meta = (ShowOnlyInnerProperties))
	FGenericDisplayInfo DisplayInfo;

	/** Rarity of the item. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "Rarity != EItemRarity::None", EditConditionHides))
	EItemRarity Rarity = EItemRarity::None;

	/** Effect applied to character when adding item to inventory. */
	UPROPERTY(EditDefaultsOnly, Instanced, NoClear)
	TObjectPtr<UItemEffect> Effect;

	/** Tags used by the item. Tags are used when generating items to increase the chance of generating items with similar tags. */
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Item"))
	FGameplayTagContainer Tags;

	/** Index of the item. Used by save data. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "Index >= 0", EditConditionHides))
	int32 Index = -1;

	/** Whether multiple stacks of the same item can be added to the same character. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "bStackable", EditConditionHides))
	bool bStackable = false;

	/** Number of stacks of the item. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "bStackable", EditConditionHides))
	mutable uint8 Stacks = 0;

	/** Disabled items remain in the character's inventory, but don't apply their effects. */
	UPROPERTY(VisibleInstanceOnly, meta = (EditCondition = "bDisabled", EditConditionHides))
	bool bDisabled = false;

};
