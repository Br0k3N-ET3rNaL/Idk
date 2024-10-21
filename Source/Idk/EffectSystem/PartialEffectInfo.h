// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "PartialEffectInfo.generated.h"

enum class EAddEffectLocation : uint8;
enum class EEffectStage : uint8;
enum class EEffectType : uint8;
class UBonusEffect;

/** Information representing a partial effect. */
USTRUCT()
struct FPartialEffectInfo final
{
	GENERATED_BODY()

	friend class UPartialEffectAddingItemEffect;
	friend class FPartialEffectAddingEffectBuilder;

public:
	/** Get the location to add the partial effect to. */
	UE_NODISCARD EAddEffectLocation GetLocation() const;

	/** Get the stage to add the partial effect to. */
	UE_NODISCARD EEffectStage GetStage() const;

	/** Get the type of effects to connect to the partial effect. */
	UE_NODISCARD EEffectType GetTypeToAddEffectTo() const;

	/** Get The effect to add. */
	UE_NODISCARD const UBonusEffect& GetEffectToAdd() const;
	
private:
	/** Where to add the partial effect to. */
	UPROPERTY(EditAnywhere, meta = (ValidEnumValues = "BasicAttack, Abilities"))
	EAddEffectLocation Location = EAddEffectLocation();

	/** Which stage to add the partial effect to. Will be connected to effects in the previous stage. */
	UPROPERTY(EditAnywhere, meta = (ValidEnumValues = "Target, Callback"))
	EEffectStage Stage = EEffectStage();

	/** Type of effects to connect to the partial effect. */
	UPROPERTY(EditAnywhere, meta = (ValidEnumValues = "Damage, Healing"))
	EEffectType TypeToAddEffectTo = EEffectType();

	/** The effect to add. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TObjectPtr<UBonusEffect> EffectToAdd;

};

