// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Templates/Function.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>

#include "StatusEffect.generated.h"

enum class EDataValidationResult : uint8;
enum class EEffectType : uint8;
class FDataValidationContext;
class FName;
class FText;
class UAttributeSystemComponent;
class UObject;
class USimpleEffect;
class UStatusEffectBlueprintBase;
class UStatusEffectInternal;
struct FGenericDisplayInfo;
struct FPropertyChangedEvent;

/** Effect that can apply persistent effects and repeatedly apply instantaneous effects for a duration. */
UCLASS()
class UStatusEffect final : public USingleStageEffect
{
	GENERATED_BODY()

	friend class FBlueprintBasedStatusEffectBuilder;
	friend class FUniqueStatusEffectBuilder;
	
public:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

	//~ Begin USingleStageEffect Interface.
	virtual void InitDescriptions(UAttributeSystemComponent& AttributeSystem) override;
	virtual void ApplyEffect(FApplyEffectParams& Params) const override;
	UE_NODISCARD virtual TSet<FEffectId> GetSaveIds() const override;
	UE_NODISCARD virtual TSet<FUseIdInfo> GetUseIds() const override;
	UE_NODISCARD virtual bool SavesBeforeUsingIds(const TSet<FEffectId>& Ids) const override;
	virtual void SwapIds(const TMap<FEffectId, FEffectId>& IdMap) override;
	virtual void GetEffectsByType(const EEffectType EffectType, TArray<USimpleEffect*>& OutEffects) override;
	virtual void GetTooltipParams(const FApplyEffectParams& EffectParams, TMap<FName, FText>& StatusEffectParams, TMap<EEffectType, double>& EffectValueParams) const override;
	virtual void RemoveEffectsByUseId(const FEffectId UseId, TFunctionRef<void()> RemoveSelf) override;
	//~ End USingleStageEffect Interface

	/**
	 * Apply any instantaneous effects to the target.
	 * 
	 * @param Target	Target to apply effects to.
	 * @param Stacks	Current number of stacks.
	 * @param Interval	Amount of time since the last time the status effect was activated.
	 */
	void ActivateEffect(UAttributeSystemComponent* Target, const uint8 Stacks, const double Interval) const;

	/** Apply any persistent effects to the target. */
	void ApplyPersistentEffects(UAttributeSystemComponent* Target) const;

	/**
	 * Remove any persistent effects from the target.
	 * 
	 * @param Target	Target to remove effects from. 
	 * @param Stacks	Number of stacks to remove.
	 */
	void RemoveAttributeAlteringEffects(UAttributeSystemComponent* Target, const uint8 Stacks) const;

	/** Get the status effect's name. */
	UE_NODISCARD const FName& GetName() const;

	/** Get the information used to display the status effect. */
	UE_NODISCARD const FGenericDisplayInfo& GetDisplayInfo() const;

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/** Create the internal status effect from the BlueprintClass. */
	void CreateBlueprintBasedStatusEffect();

	/** Whether to use a blueprint based status effect or a unique status effect. */
	UPROPERTY(EditDefaultsOnly)
	bool bUseBlueprintClass = false;

	/** Blueprint class of an existing status effect to use. */
	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly, NoResetToDefault, EditCondition = "bUseBlueprintClass", EditConditionHides))
	TSoftClassPtr<UStatusEffectBlueprintBase> BlueprintClass;

	/** Unique status effect. */
	UPROPERTY(EditAnywhere, Instanced, meta = (EditCondition = "!bUseBlueprintClass"))
	TObjectPtr<UStatusEffectInternal> StatusEffect;

};
