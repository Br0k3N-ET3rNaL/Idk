// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/IdkAttribute.h"

#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <UObject/NameTypes.h>

FAttribute::FAttribute(const bool bTreatAsModifier)
	: bTreatAsModifier(bTreatAsModifier)
{
}

FAttribute::FAttribute(const FAttribute& Other)
	: FAttribute(Other.AttributeType, Other.BaseValue, Other.Bonus, Other.MultiplierBonus, Other.bTreatAsModifier)
{
}

FAttribute& FAttribute::operator+=(const FAttribute& Other)
{
	check(!bTreatAsModifier);
	check(Other.bTreatAsModifier);
	check(AttributeType == Other.AttributeType);

	AddBonus(Other.Bonus);
	AddMultiplierBonus(Other.MultiplierBonus);

	return *this;
}

FAttribute FAttribute::operator*(const double Multiplier) const
{
	return FAttribute(AttributeType, BaseValue, Bonus * Multiplier, MultiplierBonus * Multiplier, bTreatAsModifier);
}

void FAttribute::Init(const EAttributeType InAttribute, const double InBaseValue, const double InBonus, const double InMultiplierBonus)
{
	AttributeType = InAttribute;
	BaseValue = InBaseValue;
	bCanHaveBaseValue = !bTreatAsModifier && FAttribute::CanAttributeHaveBaseValue(InAttribute);
	bCanHaveBonus = FAttribute::CanAttributeHaveBonus(InAttribute);
	bCanHaveMultiplier = FAttribute::CanAttributeHaveMultiplierBonus(InAttribute);
	Bonus = InBonus;
	MultiplierBonus = InMultiplierBonus;

	ResetUnusedValues();
}

void FAttribute::Update(const double InBaseValue)
{
	BaseValue = InBaseValue;
}

void FAttribute::AddBonus(const double InBonus)
{
	Bonus = (bCanHaveBonus) ? Bonus + InBonus : Bonus;
}

void FAttribute::AddMultiplierBonus(const double InMultiplierBonus)
{
	MultiplierBonus = (bCanHaveMultiplier) ? MultiplierBonus + InMultiplierBonus : MultiplierBonus;
}

double FAttribute::GetFinalValue() const
{
	return ApplyModifier(BaseValue);
}

double FAttribute::ApplyModifier(const double Value) const
{
	return FMath::Max(FMath::Max(Value + Bonus, MinPreMultValue) * FMath::Max(1.0 + MultiplierBonus, MinMultiplier), MinPostMultValue);
}

EAttributeType FAttribute::GetAttributeType() const
{
	return AttributeType;
}

double FAttribute::GetBaseValue() const
{
	return BaseValue;
}

double FAttribute::GetBonus() const
{
	return Bonus;
}

double FAttribute::GetMultiplierBonus() const
{
	return MultiplierBonus;
}

bool FAttribute::CanHaveBaseValue() const
{
	return bCanHaveBaseValue;
}

bool FAttribute::CanHaveBonus() const
{
	return bCanHaveBonus;
}

bool FAttribute::CanHaveMultiplierBonus() const
{
	return bCanHaveMultiplier;
}

#if WITH_EDITOR
void FAttribute::PostEditChangeProperty(const FName& PropertyName, const bool bInTreatAsModifier)
{
	bTreatAsModifier = bInTreatAsModifier;

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(FAttribute, AttributeType)))
	{
		bCanHaveBaseValue = !bTreatAsModifier && FAttribute::CanAttributeHaveBaseValue(AttributeType);
		bCanHaveBonus = FAttribute::CanAttributeHaveBonus(AttributeType);
		bCanHaveMultiplier = FAttribute::CanAttributeHaveMultiplierBonus(AttributeType);

		ResetUnusedValues();
	}
}
#endif

FAttribute::FAttribute(const EAttributeType AttributeType, const double BaseValue, const double Bonus, const double MultiplierBonus, const bool bTreatAsModifier)
	: AttributeType(AttributeType), BaseValue(BaseValue), Bonus(Bonus), MultiplierBonus(MultiplierBonus), bTreatAsModifier(bTreatAsModifier)
{
	bCanHaveBaseValue = !bTreatAsModifier && FAttribute::CanAttributeHaveBaseValue(AttributeType);
	bCanHaveBonus = FAttribute::CanAttributeHaveBonus(AttributeType);
	bCanHaveMultiplier = FAttribute::CanAttributeHaveMultiplierBonus(AttributeType);

	ResetUnusedValues();
}

void FAttribute::ResetUnusedValues()
{
	BaseValue = (bCanHaveBaseValue) ? BaseValue : 0.0;
	Bonus = (bCanHaveBonus) ? Bonus : 0.0;
	MultiplierBonus = (bCanHaveMultiplier) ? MultiplierBonus : 0.0;
}
