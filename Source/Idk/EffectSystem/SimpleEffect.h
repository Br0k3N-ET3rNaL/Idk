#pragma once

#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Containers/UnrealString.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/Set.h>
#include <HAL/Platform.h>
#include <Templates/Function.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>

#include "SimpleEffect.generated.h"

enum class EDataValidationResult : uint8;
class FDataValidationContext;
class FText;
class UAttributeSystemComponent;

/** Information about how a variable value is used by a simple effect. */
USTRUCT()
struct FEffectVariableValueInfo final
{
	GENERATED_BODY()

public:
	FEffectVariableValueInfo() = default;

	/**
	 * @param Weight				Multiplied by the variable value to get the final value.
	 * @param bUseValueAsMultiplier	Whether to use the variable value as a multiplier bonus or a bonus.
	 * @param bUseLocalValue		Whether to use a variable value from the current stage or the previous stage.			
	 */
	FEffectVariableValueInfo(const double Weight, const bool bUseValueAsMultiplier = false, const bool bUseLocalValue = false);

	/** Get the weight that will be multiplied by the variable value to get the final value. */
	UE_NODISCARD double GetWeight() const;

	/** Check if the variable value should be used as a mulitplier bonus or a bonus. */
	UE_NODISCARD bool UsesValueAsMultiplierBonus() const;

	/** Check if the variable value comes from the current stage or the previous stage. */
	UE_NODISCARD bool UsesLocalValue() const;

protected:
	/** Multiplied by the variable value to get the final value. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	double Weight = 0.0;

	/** Whether to use the variable value as a multiplier bonus or a bonus. */
	UPROPERTY(EditAnywhere)
	bool bUseValueAsMultiplierBonus = false;

	/** Whether to use a variable value from the current stage or the previous stage. */
	UPROPERTY(EditAnywhere)
	bool bUseLocalValue = true;

};

/** Represents a single instantaneous effect. */
UCLASS()
class USimpleEffect final : public USingleStageEffect
{
	GENERATED_BODY()

	friend class FSimpleEffectBuilder;

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

	/**
	 * Add a variable value to use when applying the effect.
	 * 
	 * @param UseId	ID of the variable value to use. 
	 * @param Info	Information about how to the variable value should be used. @see FEffectVariableValueInfo
	 */
	void AddVariableToUse(const uint8 UseId, const FEffectVariableValueInfo Info);

	/** Add a bonus ID to save the effect's value to when applying the effect. */
	void AddBonusSaveId(const uint8 BonusSaveId);

	/** Remove a bonus ID that was previously added. */
	void RemoveBonusSaveId(const uint8 BonusSaveId);

	/** Get the type of the modifier that should be applied to the effect's value. */
	UE_NODISCARD EAttributeType GetModifierType() const;

	/** Name that represents any/all status effects. */
	static const FName AnyStatusEffect;

	/** Name to use to generate unique effect names. */
	static const FString EffectClassName;

private:
	/**
	 * Apply the effect.
	 * 
	 * @param Params	Information that determines how the effect should be applied. @see FApplyEffectParams 
	 */
	double InternalApplyEffect(FApplyEffectParams& Params) const;

	/**
	 * Get the effect's current value. @note Used to update tooltips based on value modifiers.
	 * 
	 * @param Params	Information used to apply modifiers.
	 * @return			The effects value after applying modifiers.
	 */
	UE_NODISCARD double GetEffectValue(const FApplyEffectParams& Params) const;

	/** Type of the effect. */
	UPROPERTY(EditAnywhere)
	EEffectType Type = EEffectType();

	/** Base value of the effect. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EEffectType::Damage || Type == EEffectType::Healing || Type == EEffectType::Push", EditConditionHides))
	double BaseValue = 0.0;

	/** Name of a status effect or item. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EEffectType::GetStatusEffectStacks || (Type == EEffectType::RemoveStatusEffect && !bRemoveAllStatusEffects) || Type == EEffectType::DisableItem", EditConditionHides))
	FName NameForEffect = FName();

	/** ID used to save the effect's value when it is applied. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Save ID", ShowOnlyInnerProperties, EditCondition = "Type != EEffectType::Push && Type != EEffectType::DisableItem", EditConditionHides))
	FEffectId SaveId = FEffectId::NoId;

	/** Additional IDs to save the effect's value to when it is applied. */
	UPROPERTY(VisibleInstanceOnly)
	TSet<FEffectId> BonusSaveIds;

	/** Maps IDs to information about how the value with the corresponding ID should be applied to the effect's value. */
	UPROPERTY(EditAnywhere, meta = (ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FEffectId, FEffectVariableValueInfo> ValuesToUse;

	/** Whether to remove all status effects or only the one with the specified name. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EEffectType::RemoveStatusEffect", EditConditionHides))
	bool bRemoveAllStatusEffects = false;

	/** Whether modifiers should be used when applying the effect. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EEffectType::Damage || Type == EEffectType::Healing"))
	bool bUseModifiers = true;

	/** Multiplied by the value of a push effect to get the amount of force applied. */
	static constexpr double PushMultiplier = 50000000.0;

};
