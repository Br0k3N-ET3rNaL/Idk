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

#include "SingleStageEffectGroup.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FName;
class FText;
class UAttributeSystemComponent;
class UObject;
class USimpleEffect;

/** Represents a group of single stage effects. */
UCLASS()
class USingleStageEffectGroup : public USingleStageEffect
{
	GENERATED_BODY()

	template<class EffectClass, class BuilderType>
	friend class TSingleStageEffectGroupBuilder;
	
public:
	//~ Begin UEffect Interface.
#if WITH_EDITOR
	UE_NODISCARD virtual EDataValidationResult IsEffectValid(FDataValidationContext& Context, const EEffectValidationFlags Flags = EEffectValidationFlags::None) const override;
#endif
	UE_NODISCARD virtual UEffect* DuplicateEffectInternal(UObject* Outer, const FName& Name) const override;
	//~ End UEffect Interface

public:
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

	/** Declares the effect group as only using variable values from itself. */
	void SetSelfContained();

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

protected:
	/**
	 * Copy subeffects over to a newly created effect group.
	 * 
	 * @param NewEffect Effect group to copy subeffects to.
	 */
	void DeepCopyEffectGroup(UObject* Outer, USingleStageEffectGroup* NewEffect) const;

	/** List of subeffects. */
	UPROPERTY(EditAnywhere, Instanced, NoClear)
	TArray<USingleStageEffect*> Effects;

	/** Whether the effect group only uses variable values from itself. */
	UPROPERTY(VisibleAnywhere)
	bool bSelfContained = false;
};
