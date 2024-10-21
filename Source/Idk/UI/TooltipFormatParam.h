// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <UObject/NameTypes.h>

enum class EEffectStage : uint8;
enum class EEffectType : uint8;

/** 
 * Contains functions used when formatting tooltips. 
 * @see UMultiStageEffect::GetTooltipParams, UStatusEffectInternal::GetTooltipParams 
 */
struct FTooltipFormatParam final
{
public:
	FTooltipFormatParam() = delete;

	/** Check if the parameter represents a value from a multistage effect. */
	UE_NODISCARD static bool IsMultiStageEffectValue(const FString& Param);

	/** Check if the parameter represents a value from a single stage effect. */
	UE_NODISCARD static bool IsSingleStageEffectValue(const FString& Param);

	/** Check if the parameter represents a status effect. */
	UE_NODISCARD static bool IsStatusEffect(const FString& Param);

	/** Check if the parameter represents a duration. */
	UE_NODISCARD static bool IsDuration(const FString& Param);

	/** Check if the parameter represents an interval. */
	UE_NODISCARD static bool IsInterval(const FString& Param);

	/** Extract the effect stage from a multistage effect parameter. */
	UE_NODISCARD static EEffectStage GetEffectStage(const FString& Param);

	/** Extract the effect type from a multistage or single stage effect parameter. */
	UE_NODISCARD static EEffectType GetEffectType(const FString& Param);

	/** Extract the status effect name from a status effect parameter. */
	UE_NODISCARD static FName GetStatusEffectName(const FString& Param);

	/** Get the parameter for a status effect with the specified name. */
	UE_NODISCARD static FString FromStatusEffect(const FName& StatusEffectName);

	/** Get the parameter for a value from a multistage effect with the specified stage and effect type. */
	UE_NODISCARD static FString FromMultiStageEffectValue(const EEffectStage Stage, const EEffectType EffectType);

	/** Get the parameter for a value from a single stage effect with the specified effect type. */
	UE_NODISCARD static FString FromSingleStageEffectValue(const EEffectType EffectType);

private:
	/** Maps effect stages to their string representations. */
	static const TMap<EEffectStage, FString> EffectStageStrings;

	/** Maps effect types to their string representations. */
	static const TMap<EEffectType, FString> EffectTypeStrings;

	/** Prefix used by status effect parameters. */
	static const FString StatusEffectPrefix;

	/** String representing a duration parameter. */
	static const FString DurationString;

	/** String representing an interval parameter. */
	static const FString IntervalString;

};
