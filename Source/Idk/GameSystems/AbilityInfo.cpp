// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/AbilityInfo.h"

#include "Idk/DataValidationHelper.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>

#if WITH_EDITOR
EDataValidationResult FAbilityTargetingInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if (AreaType == EAbilityAreaType::None)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Area type is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!bRangedTargeting && bRangedActivation)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Abilities that used ranged activation must also use ranged targeting.")));

		Result = EDataValidationResult::Invalid;
	}

	if ((AreaType == EAbilityAreaType::Circle || AreaType == EAbilityAreaType::Cone) && Radius <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Radius must be positive for circles and cones.")));

		Result = EDataValidationResult::Invalid;
	}

	if (AreaType == EAbilityAreaType::Cone && HalfAngle <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Half-angle must be positive for cones.")));

		Result = EDataValidationResult::Invalid;
	}

	if (AreaType == EAbilityAreaType::Rectangle && Width <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Width must be positive for rectangles.")));

		Result = EDataValidationResult::Invalid;
	}

	if (AreaType == EAbilityAreaType::Rectangle && Length <= 0.0)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityTargetingInfo: Length must be positive for cones.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

EAbilityAreaType FAbilityTargetingInfo::GetAreaType() const
{
	return AreaType;
}

bool FAbilityTargetingInfo::UsesRangedTargeting() const
{
	return bRangedTargeting;
}

bool FAbilityTargetingInfo::UsesRangedActivation() const
{
	return bRangedActivation;
}

double FAbilityTargetingInfo::GetRange() const
{
	return Range;
}

double FAbilityTargetingInfo::GetRadius() const
{
	check(AreaType == EAbilityAreaType::Circle || AreaType == EAbilityAreaType::Cone);

	return Radius;
}

double FAbilityTargetingInfo::GetHalfAngle() const
{
	check(AreaType == EAbilityAreaType::Cone);

	return HalfAngle;
}

double FAbilityTargetingInfo::GetWidth() const
{
	check(AreaType == EAbilityAreaType::Rectangle);

	return Width;
}

double FAbilityTargetingInfo::GetLength() const
{
	check(AreaType == EAbilityAreaType::Rectangle);

	return Length;
}

bool FAbilityTargetingInfo::HandlesCollision() const
{
	return AreaType != EAbilityAreaType::Custom;
}

bool FAbilityTargetingInfo::HasInfiniteRange() const
{
	return Range < 0.0;
}

FAbilityTargetingInfo FAbilityTargetingInfo::InitCircle(const double Radius, const double Range, const bool bRangedTargeting, const bool bRangedActivation)
{
	check(Radius >= 0.0);
	check(!bRangedActivation || bRangedTargeting);

	FAbilityTargetingInfo AbilityTargetingInfo;

	AbilityTargetingInfo.AreaType = EAbilityAreaType::Circle;

	AbilityTargetingInfo.Radius = Radius;

	AbilityTargetingInfo.Range = Range;
	AbilityTargetingInfo.bRangedTargeting = bRangedTargeting;
	AbilityTargetingInfo.bRangedActivation = bRangedActivation;

	return AbilityTargetingInfo;
}

FAbilityTargetingInfo FAbilityTargetingInfo::InitCone(const double Radius, const double HalfAngle, const double Range, const bool bRangedTargeting, const bool bRangedActivation)
{
	check(Radius >= 0.0);
	check(HalfAngle >= 0.0);
	check(!bRangedActivation || bRangedTargeting);

	FAbilityTargetingInfo AbilityTargetingInfo;

	AbilityTargetingInfo.AreaType = EAbilityAreaType::Cone;

	AbilityTargetingInfo.Radius = Radius;
	AbilityTargetingInfo.HalfAngle = HalfAngle;

	AbilityTargetingInfo.Range = Range;
	AbilityTargetingInfo.bRangedTargeting = bRangedTargeting;
	AbilityTargetingInfo.bRangedActivation = bRangedActivation;

	return AbilityTargetingInfo;
}

FAbilityTargetingInfo FAbilityTargetingInfo::InitRectangle(const double Width, const double Length, const double Range, const bool bRangedTargeting, const bool bRangedActivation)
{
	check(Width >= 0.0);
	check(Length >= 0.0);
	check(!bRangedActivation || bRangedTargeting);

	FAbilityTargetingInfo AbilityTargetingInfo;

	AbilityTargetingInfo.AreaType = EAbilityAreaType::Rectangle;

	AbilityTargetingInfo.Width = Width;
	AbilityTargetingInfo.Length = Length;

	AbilityTargetingInfo.Range = Range;
	AbilityTargetingInfo.bRangedTargeting = bRangedTargeting;
	AbilityTargetingInfo.bRangedActivation = bRangedActivation;

	return AbilityTargetingInfo;
}

FAbilityTargetingInfo FAbilityTargetingInfo::InitCustom(const double Range, const bool bRangedTargeting, const bool bRangedActivation)
{
	check(!bRangedActivation || bRangedTargeting);

	FAbilityTargetingInfo AbilityTargetingInfo;

	AbilityTargetingInfo.AreaType = EAbilityAreaType::Custom;

	AbilityTargetingInfo.Range = Range;
	AbilityTargetingInfo.bRangedTargeting = bRangedTargeting;
	AbilityTargetingInfo.bRangedActivation = bRangedActivation;

	return AbilityTargetingInfo;
}

#if WITH_EDITOR
const FText FAbilityInfo::DataValidationPrefix = FText::FromString(TEXT("FAbilityInfo: "));

EDataValidationResult FAbilityInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	FDataValidationContext TargetingInfoContext;

	if (TargetingInfo.IsDataValid(TargetingInfoContext) == EDataValidationResult::Invalid)
	{
		FDataValidationHelper::AddPrefixAndAppendIssues(DataValidationPrefix, Context, TargetingInfoContext);

		Result = EDataValidationResult::Invalid;
	}

	FDataValidationContext EffectContext;

	if (!Effect)
	{
		Context.AddError(FText::FromString(TEXT("FAbilityInfo: Effect is not set.")));

		Result = EDataValidationResult::Invalid;
	}
	else if (Effect->IsDataValid(EffectContext) == EDataValidationResult::Invalid)
	{
		FDataValidationHelper::AddPrefixAndAppendIssues(DataValidationPrefix, Context, EffectContext);

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void FAbilityInfo::InitDescription(UAttributeSystemComponent& AttributeSystem)
{
	if (Effect)
	{
		Effect->InitDescriptions(AttributeSystem);
	}

	DisplayInfo.GetDescriptionFormatArgsDelegate.BindUObject(Effect, &UMultiStageEffect::GetTooltipParams, &AttributeSystem, Duration, HitRate);

	DisplayInfo.InitDescription(AttributeSystem);
}

const FName& FAbilityInfo::GetName() const
{
	return Name;
}

const FGenericDisplayInfo& FAbilityInfo::GetDisplayInfo() const
{
	return DisplayInfo;
}

bool FAbilityInfo::HasCooldown() const
{
	return Cooldown != 0.0;
}

bool FAbilityInfo::IsCooldownInfinite() const
{
	return Cooldown < 0.0;
}

double FAbilityInfo::GetCooldown() const
{
	return Cooldown;
}

const FAbilityTargetingInfo& FAbilityInfo::GetTargetingInfo() const
{
	return TargetingInfo;
}

const UMultiStageEffect& FAbilityInfo::GetEffect() const
{
	return *Effect;
}

UMultiStageEffect& FAbilityInfo::GetEffect()
{
	return *Effect;
}

double FAbilityInfo::GetHitRate() const
{
	return HitRate;
}

double FAbilityInfo::GetDuration() const
{
	return Duration;
}

double FAbilityInfo::GetPerHitMultiplier() const
{
	return (Duration > 0.0) ? HitRate / Duration : 1.0;
}
