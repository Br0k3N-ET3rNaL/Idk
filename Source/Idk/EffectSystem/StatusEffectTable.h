// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/Map.h>
#include <Containers/UnrealString.h>
#include <Engine/DataAsset.h>
#include <HAL/Platform.h>
#include <Math/Color.h>
#include <Misc/Optional.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

#include "StatusEffectTable.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class UAttributeSystemComponent;
class UStatusEffect;
class UStatusEffectBlueprintBase;
struct FKeywordInfo;

/** Information used to determine how a status effect from the status effect table is displayed in a tooltip. */
USTRUCT()
struct FStatusEffectTooltipInfo final
{
	GENERATED_BODY()

public:
	/** Initialize the status effect's description based on modifiers from the specified attribute system. */
	void InitDescription(UAttributeSystemComponent& AttributeSystem) const;

	/** Get the name of the status effect. */
	UE_NODISCARD FString GetStatusEffectName() const;

	/** Get the information used to display the status effect name within a tooltip. */
	UE_NODISCARD FKeywordInfo GetKeywordInfo() const;

	/** Get the status effect's class. */
	UE_NODISCARD TSubclassOf<UStatusEffectBlueprintBase> GetClass() const;

	/** Check whether the status effect's class is set. */
	UE_NODISCARD bool IsStatusEffectClassSet() const;

private:
	/** Class of the status effect. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStatusEffectBlueprintBase> StatusEffectClass;

	/** Color of the status effect's name within a tooltip. */
	UPROPERTY(EditDefaultsOnly)
	FLinearColor TextHighlightColor = FLinearColor::White;
};

/** Contains all status effects that are used in more than one place. */
UCLASS()
class UStatusEffectTable final : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

	/** Initialize all status effect descriptions based on modifiers from the specified attribute system. */
	void InitDescriptions(UAttributeSystemComponent& AttributeSystem) const;

	/** 
	 * Get the information used to display the status effect name within a tooltip. 
	 * 
	 * @param StatusEffectName Name of the status effect to get the information for.
	 */
	UE_NODISCARD TOptional<FKeywordInfo> GetKeywordInfo(const FString& StatusEffectName) const;

	/** Get the class of the status effect with the specified name. */
	UE_NODISCARD TSubclassOf<UStatusEffectBlueprintBase> GetStatusEffectClassByName(const FString& StatusEffectName) const;

private:
	/** Maps status effect names to their tooltip information. */
	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TMap<FString, FStatusEffectTooltipInfo> StatusEffects;

};
