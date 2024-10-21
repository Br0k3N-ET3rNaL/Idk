// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Items/Item.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/ItemEffect.h"
#include "Idk/GameSystems/Items/ItemSaveData.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/UnrealString.h>
#include <GameplayTagContainer.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/Color.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <NativeGameplayTags.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UObjectGlobals.h>

UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type1, "Item.Type1");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type2, "Item.Type2");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type3, "Item.Type3");

#if WITH_EDITOR
EDataValidationResult UItem::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	FString InvalidReason;

	FDataValidationContext EffectContext;

	if (Effect && Effect->IsEffectValid(EffectContext) == EDataValidationResult::Invalid)
	{
		const FText InvalidEffectPrefix = FText::FromString(TEXT("Effect is invalid: "));

		FDataValidationHelper::AddPrefixAndAppendIssues(InvalidEffectPrefix, Context, EffectContext);

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void UItem::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = Tags;
}

bool UItem::operator<(const UItem& Other) const
{
	// Order items by rarity then by name
	return uint8(Rarity) < uint8(Other.Rarity)
		|| (Rarity == Other.Rarity && DisplayInfo.GetDisplayName().CompareTo(Other.DisplayInfo.GetDisplayName()) < 0);
}

bool UItem::operator==(const UItem& Other) const
{
	return !Name.IsNone() && Name.IsEqual(Other.Name);
}

bool operator==(const UItem* Lhs, const UItem& Rhs)
{
	check(Lhs);

	return *Lhs == Rhs;
}

const FName& UItem::GetItemName() const
{
	return Name;
}

const FGenericDisplayInfo& UItem::GetDisplayInfo() const
{
	return DisplayInfo;
}

EItemRarity UItem::GetRarity() const
{
	return Rarity;
}

uint8 UItem::GetStacks() const
{
	return Stacks;
}

bool UItem::IsStackable() const
{
	return bStackable;
}

bool UItem::IsDisabled() const
{
	return bDisabled;
}

void UItem::InitRarity(const EItemRarity InRarity, const FLinearColor& RarityColor)
{
	check(Rarity == EItemRarity::None);
	check(InRarity != EItemRarity::None);

	Rarity = InRarity;

	DisplayInfo.InitBorderColor(RarityColor);
}

void UItem::SetIndex(const int32 InIndex)
{
	check(Index == -1);
	check(InIndex >= 0);

	Index = InIndex;
}

void UItem::EnableStacking()
{
	bStackable = true;
}

void UItem::DisableItem()
{
	check(!bDisabled);

	bDisabled = true;
}

FItemSaveData UItem::GetItemSaveData() const
{
	return FItemSaveData(Name, Rarity, Index, bDisabled);
}

bool UItem::MatchesSaveData(const FItemSaveData& SaveData) const
{
	return !Name.IsNone() && Name.IsEqual(SaveData.GetName());
}

void UItem::ApplyEffect(UAttributeSystemComponent* Target, const bool bUpdateStacks) const
{
	if (!bDisabled && Effect)
	{
		Effect->ApplyEffect(Target);
	}

	if (bUpdateStacks)
	{
		++Stacks;
	}
}

void UItem::RemoveEffect(UAttributeSystemComponent* Target, const bool bRemoveAllStacks) const
{
	if (Effect)
	{
		Effect->RemoveEffect(Target, (bRemoveAllStacks) ? Stacks : 1);
	}

	Stacks = (bRemoveAllStacks) ? 1 : Stacks - 1;
}

UItem* UItem::DuplicateItem(UObject* Outer, const UItem& Item)
{
	const FName& NewItemName = (Outer == Item.GetOuter()) 
		? MakeUniqueObjectName(Outer, UItem::StaticClass(), Item.Name) 
		: Item.Name;

	UItem* NewItem = NewObject<UItem>(Outer, NewItemName);

	NewItem->Name = Item.Name;
	NewItem->DisplayInfo = Item.DisplayInfo;
	NewItem->Rarity = Item.Rarity;
	
	if (Item.Effect)
	{
		NewItem->Effect = UEffect::DuplicateEffect(*Item.Effect, Outer, *FString::Printf(TEXT("%s_Effect"), *NewItemName.ToString()));
	}

	NewItem->Tags = Item.Tags;
	NewItem->Index = Item.Index;
	NewItem->bStackable = Item.bStackable;
	NewItem->Stacks = Item.Stacks;
	NewItem->bDisabled = Item.bDisabled;

	return NewItem;
}
