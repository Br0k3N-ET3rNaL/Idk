// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/EffectBuilder.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Templates/UnrealTemplate.h>
#include <type_traits>

enum class EItemRarity : uint8;
class FName;
class UItem;
class UItemEffect;
class UObject;
struct FGameplayTag;
struct FGenericDisplayInfo;

/** Used to build new items. */
class FItemBuilder final
{
public:
	/**
	 * Begin the building process. Used outside of constructor.
	 * 
	 * @param Outer Outer used when creating the item.
	 * @param Name	Name of the item. Must be unique.
	 * @return		An item builder instance.
	 */
	UE_NODISCARD static FItemBuilder Begin(UObject* Outer, FName&& Name);

	/**
	 * Begin the building process. Used within the constructor.
	 * 
	 * @param Outer Outer used when creating the item.
	 * @param Name	Name of the item. Must be unique.
	 * @return		An item builder instance.
	 */
	UE_NODISCARD static FItemBuilder BeginDefault(UObject* Outer, FName&& Name);

	/**
	 * Initialize the item with the supplied name and display info. 
	 * @param DisplayInfo	Information used to display the item to the player.
	 * @return				A reference to the item builder.
	 */
	UE_NODISCARD FItemBuilder&& Init(FGenericDisplayInfo&& DisplayInfo)&&;

	/**
	 * Set the item's effect.
	 * 
	 * @tparam EffectClass	Class of the effect applied to the character when adding item to inventory.
	 * @tparam BuilderType	Type of the builder used to build the effect.
	 * @param Builder		Builder used to build the effect.
	 * @return				A reference to the item builder.
	 */
	template<class EffectClass, class BuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, EffectClass>, bool> = true>
	UE_NODISCARD FItemBuilder&& SetEffect(TEffectBuilderDeferred<EffectClass, BuilderType>&& Builder)&&;

	/**
	 * Add a tag to the item.
	 * 
	 * @param Tag	Tag to add to the item. @see UItem::Tags
	 * @return		A reference to the item builder.
	 */
	UE_NODISCARD FItemBuilder&& AddTag(const FGameplayTag& Tag)&&;

	/**
	 * Finish building the item.
	 * 
	 * @return	The newly created item.
	 */
	UE_NODISCARD UItem* Complete()&&;

private:
	FItemBuilder() = default;
	FItemBuilder(UItem* Item);

	/** Item that is being built. */
	UItem* Item = nullptr;
	
};

template<class EffectClass, class BuilderType, std::enable_if_t<std::is_base_of_v<UItemEffect, EffectClass>, bool>>
inline FItemBuilder&& FItemBuilder::SetEffect(TEffectBuilderDeferred<EffectClass, BuilderType>&& Builder)&&
{
	const FString EffectName = FString::Printf(TEXT("%sEffect"), *Item->GetFName().ToString());

	Item->Effect = &Builder.Complete(EffectName);

#if WITH_EDITOR
	Item->Effect->CheckIsValidEffect(EEffectValidationFlags::InitialValidation);
#endif

	return MoveTemp(*this);
}
