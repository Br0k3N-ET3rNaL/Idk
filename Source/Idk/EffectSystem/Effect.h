// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/EnumClassFlags.h>
#include <type_traits>
#include <UObject/Class.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>
#include <UObject/UObjectThreadContext.h>

#include "Effect.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class FProperty;
class UWorld;

/** Represents the different types of simple effects. */
UENUM()
enum class EEffectType : uint8
{
	None UMETA(Hidden),

	/** Deals damage to the target. */
	Damage,

	/** Heals the target. */
	Healing,

	/** Saves the amount of health the target is missing. */
	GetMissingHealth,

	/** Saves the current health percentage of the target. */
	GetHealthPercent UMETA(DisplayName = "Get Health %"),

	/** Saves the number of stacks of a status effect on the target. */
	GetStatusEffectStacks,

	/** Removes a status effect by name from the target. */
	RemoveStatusEffect,

	/** Pushes (or pulls if negative) the target. */
	Push,

	/** Disables an item in the target's inventory. */
	DisableItem
};

/** Flags that can be used when validating an effect. */
UENUM()
enum class EEffectValidationFlags : uint8
{
	None = 0,

	/** Set when validating an effect after creation. */
	InitialValidation = 1,

	/** Set when the effect is within a partial effect's bonus effect. */
	InPartialEffect = 1 << 2,

	/** Set when the effect is within a status effect. */
	InStatusEffect = 1 << 3,
};
ENUM_CLASS_FLAGS(EEffectValidationFlags);

/** Base class for effects. */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class UEffect : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override final;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
	//~ End UObject Interface

#if WITH_EDITOR
	/**
	 * Check if the effect is valid.
	 *
	 * @param Context	Context for data validation.
	 * @param Flags		Flags used during validation.
	 * @return			True if the effect was valid, otherwise false.
	 */
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const;
#endif

	/**
	 * Peform a deep copy of the effect.
	 *
	 * @param Outer	Object used when creating the new effect (not the effect to copy).
	 * @param Name	Name of the effect to create.
	 * @return		A copy of the effect.
	 */
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const PURE_VIRTUAL(UEffect::DuplicateEffectInternal, return nullptr;);

#if WITH_EDITOR
	/**
	 * Asserts that the effect is valid.
	 * 
	 * @param Flags	Flags used during validation. 
	 */
	void CheckIsValidEffect(const EEffectValidationFlags Flags = EEffectValidationFlags::None) const;
#endif

	/**
	 * Peform a deep copy of the effect.
	 * 
	 * @tparam EffectClass	Class of effect to create.
	 * @param Effect		Effect to duplicate.
	 * @param Outer			Object used when creating the new effect (not the effect to copy).
	 * @return				A copy of the effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* DuplicateEffect(const EffectClass& Effect, UObject* Outer);

	/**
	 * Peform a deep copy of the effect.
	 * 
	 * @tparam EffectClass		Class of effect to create.
	 * @param Effect			Effect to duplicate.
	 * @param Outer				Object used when creating the new effect (not the effect to copy).
	 * @param OldOuterEffect	Effect containing the original effect.
	 * @param NewOuterEffect	Effect that will contain the duplicate effect.
	 * @return					A copy of the effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* DuplicateEffect(const EffectClass& Effect, UObject* Outer, const UEffect& OldOuterEffect, const UEffect& NewOuterEffect);

	/**
	 * Peform a deep copy of the effect.
	 *
	 * @tparam EffectClass	Class of effect to create.
	 * @param Effect		Effect to duplicate.
	 * @param Outer			Object used when creating the new effect (not the effect to copy).
	 * @param Name			Name of the duplicate effect.
	 * @return				A copy of the effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* DuplicateEffect(const EffectClass& Effect, UObject* Outer, const FName& Name);

	/**
	 * Creates an effect of the specified class.
	 * Should only be used in an object's constructor.
	 *
	 * @tparam EffectClass	Class of effect to create.
	 * @param Outer			Object used to create the effect.
	 * @return				The newly created effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* CreateEffectDefault(UObject* Outer);

	/**
	 * Creates an effect of the specified class.
	 * Should only be used in an object's constructor.
	 *
	 * @tparam EffectClass	Class of effect to create.
	 * @param Outer			Object used to create the effect.
	 * @param Name			Name of the effect to create.
	 * @return				The newly created effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* CreateEffectDefault(UObject* Outer, const FName& Name);

	/**
	 * Creates an effect of the specified class.
	 * 
	 * @tparam EffectClass	Class of effect to create.
	 * @param Outer			Object used to create the effect.
	 * @return				The newly created effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* CreateEffect(UObject* Outer);

	/**
	 * Creates an effect of the specified class.
	 *
	 * @tparam EffectClass	Class of effect to create.
	 * @param Outer			Object used to create the effect.
	 * @param Name			Name of the effect to create.
	 * @return				The newly created effect.
	 */
	template<class EffectClass>
	UE_NODISCARD static EffectClass* CreateEffect(UObject* Outer, const FName& Name);

	/** Get the name for a specific effect class. */
	template<class EffectClass>
	UE_NODISCARD static const FString& GetEffectClassName();

protected:
	/**
	 * Create an effect with the same class and object flags as the effect. 
	 * 
	 * @tparam EffectClass	Class of effect to create.
	 * @param Outer			Object used to create the effect.
	 * @param Name			Name of the effect to create.
	 * @return				The newly created effect.
	 */
	template<class EffectClass>
	UE_NODISCARD EffectClass* DuplicateBaseEffect(UObject* Outer, const FName& Name) const;

};

template<class EffectClass>
inline EffectClass* UEffect::DuplicateEffect(const EffectClass& Effect, UObject* Outer)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);
	check(Effect.IsA<EffectClass>());
	check(Outer != Effect.GetOuter());

	return CastChecked<EffectClass>(Effect.DuplicateEffectInternal(Outer, MakeUniqueObjectName(Outer, EffectClass::StaticClass())));
}

template<class EffectClass>
inline EffectClass* UEffect::DuplicateEffect(const EffectClass& Effect, UObject* Outer, const UEffect& OldOuterEffect, const UEffect& NewOuterEffect)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);
	check(Effect.IsA<EffectClass>());

	const FString OldOuterName = OldOuterEffect.GetFName().ToString();
	const FString OriginalName = Effect.GetFName().ToString();
	const FString NewOuterName = NewOuterEffect.GetFName().ToString();

	const FString NewName = OriginalName.Replace(*OldOuterName, *NewOuterName);

	return CastChecked<EffectClass>(Effect.DuplicateEffectInternal(Outer, *NewName));
}

template<class EffectClass>
inline EffectClass* UEffect::DuplicateEffect(const EffectClass& Effect, UObject* Outer, const FName& Name)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);
	check(Effect.IsA<EffectClass>());
	check(Outer != Effect.GetOuter() || !Effect.GetFName().IsEqual(Name));

	return CastChecked<EffectClass>(Effect.DuplicateEffectInternal(Outer, Name));
}

template<class EffectClass>
inline EffectClass* UEffect::CreateEffectDefault(UObject* Outer)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	return UEffect::CreateEffectDefault<EffectClass>(Outer, MakeUniqueObjectName(Outer, EffectClass::StaticClass()));
}

template<class EffectClass>
inline EffectClass* UEffect::CreateEffectDefault(UObject* Outer, const FName& Name)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	return Outer->CreateDefaultSubobject<EffectClass>(Name);
}

template<class EffectClass>
inline EffectClass* UEffect::CreateEffect(UObject* Outer)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	return UEffect::CreateEffect<EffectClass>(Outer, MakeUniqueObjectName(Outer, EffectClass::StaticClass()));
}

template<class EffectClass>
inline EffectClass* UEffect::CreateEffect(UObject* Outer, const FName& Name)
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	return NewObject<EffectClass>(Outer, Name, Outer->GetMaskedFlags(RF_PropagateToSubObjects));
}

template<class EffectClass>
inline const FString& UEffect::GetEffectClassName()
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);

	return EffectClass::EffectClassName;
}

template<class EffectClass>
inline EffectClass* UEffect::DuplicateBaseEffect(UObject* Outer, const FName& Name) const
{
	static_assert(std::is_base_of<UEffect, EffectClass>::value);
	check(UClass::FindCommonBase(EffectClass::StaticClass(), GetClass()) == EffectClass::StaticClass());

	FObjectInitializer* CurrentInitializer = FUObjectThreadContext::Get().TopInitializer();

	if (CurrentInitializer && CurrentInitializer->GetObj() == Outer)
	{
		return Outer->CreateDefaultSubobject<EffectClass>(Name);
	}

	return NewObject<EffectClass>(Outer, Name, GetFlags());
}
