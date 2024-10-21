// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectIdSet.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include <Components/ActorComponent.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/SparseArray.h>
#include <Delegates/Delegate.h>
#include <Delegates/DelegateCombinations.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <UObject/SoftObjectPtr.h>
#include <UObject/UnrealType.h>

#include "AttributeSystemComponent.generated.h"

enum class EAddEffectLocation : uint8;
enum class EConditionComparator : uint8;
enum class EConditionType : uint8;
enum class EDataValidationResult : uint8;
enum class EEffectStage : uint8;
enum class EEffectType : uint8;
class AIdkCharacter;
class FDataValidationContext;
class UAttributeDisplayWidget;
class UBonusEffect;
class UCharacterMovementComponent;
class UCurveFloat;
class UDamageNumberComponent;
class UHealthAndStatusBarWidget;
class UIdkHealthComponent;
class UMultiStageBonusEffect;
class UMultiStageEffect;
class UPartialEffectAddingItemEffect;
class USimpleEffect;
class USingleStageEffect;
class UStatusEffectComponent;
class UStatusEffectInternal;
struct FEffectVariableValueInfo;
struct FPartialEffectInfo;

DECLARE_DELEGATE_OneParam(FSetStunnedDelegate, bool);
DECLARE_DELEGATE_RetVal(uint8, FGetLevelDelegate);
DECLARE_DELEGATE_OneParam(FDisableInventoryItemByNameDelegate, const FName&);

/** Ids used by partial effect adding effects. */
struct FPartialEffectIdPair final
{
	FPartialEffectIdPair() = default;

	/** Id of the bonuse effect used by the partial effect adding effect. */
	FEffectId BonusEffectId;

	/** Use/save id used by individual effects within the bonus effect. */
	FEffectId SaveId;
};

DECLARE_DELEGATE_TwoParams(FApplyPartialEffectToAbilityDelegate, UMultiStageEffect&, FPartialEffectIdPair&);
DECLARE_DELEGATE_TwoParams(FRemovePartialEffectFromAbilityDelegate, UMultiStageEffect&, const FPartialEffectIdPair&);

/** Information used to reapply or remove previously applied partial effect adding effects. */
struct FAppliedPartialEffectInfo final
{
	/** 
	 * Delegate that applies the partial effect to a supplied ability effect.
	 * 
	 * @param Effect Effect to apply partial effect to.
	 * @param IdPair Pair of ids used by the partial effect.
	 */
	FApplyPartialEffectToAbilityDelegate ApplyPartialEffectDelegate;

	/**
	 * Delegate that remove the partial effect from a supplied ability effect.
	 * 
	 * @param Effect Effect to remove partial effect from.
	 * @param IdPair Pair of ids used by the partial effect.
	 */
	FRemovePartialEffectFromAbilityDelegate RemovePartialEffectDelegate;

	/** Maps the indices of ability effects to the pair of ids used by the partial effect for the specific ability effect. */
	TMap<int32, FPartialEffectIdPair> AbilityIdMap;
};

/** Struct containing information for all the character's attributes. */
USTRUCT()
struct FDefaultAttributes final
{
	GENERATED_BODY()

public:
	FDefaultAttributes();

	UE_NODISCARD FAttribute& operator[](const EAttributeType AttributeType);
	UE_NODISCARD const FAttribute& operator[](const EAttributeType AttributeType) const;

	/** Check if there is an attribute with the given attribute type. */
	UE_NODISCARD bool Contains(const EAttributeType AttributeType) const;

	/** Get information of all the character's attributes. */
	UE_NODISCARD const TArray<FAttribute>& GetAttributes() const;

private:
	/** All the character's attributes. */
	UPROPERTY(EditAnywhere, EditFixedSize, meta = (TitleProperty = "AttributeType", EditCondition = "false"))
	TArray<FAttribute> Attributes;

	/** Maps attribute types to the index of the attribute information with the same type. */
	TMap<EAttributeType, int32> AttributeToIndex;

};

/** Component that manages a character's attributes and handles interactions with the effect system. */
UCLASS(ClassGroup = (Custom), NotBlueprintable, AutoExpandCategories = ("Idk|Character"), meta = (BlueprintSpawnableComponent))
class UAttributeSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeSystemComponent();

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;
	//~ End UActorComponent Interface

	/** Initialize the base health multiplier. @see BaseHealthMultiplier */
	void InitBaseHealthMult(const double InBaseHealthMultiplier);

	/** Initialize base movement speed. */
	void InitBaseMoveSpeed(const double MoveSpeed);

	/** Initialize max health based on the character's level. */
	void InitHealth();

	/** Set the health component. */
	void SetHealthComponent(UIdkHealthComponent& InHealthComponent);

	/** Set the character movement component. */
	void SetMovementComponent(UCharacterMovementComponent& InMovementComponent);

	/** Set the component used to manage status effects. */
	void SetStatusEffectComponent(UStatusEffectComponent& InStatusEffectComponent);

	/** Set the component used to spawn floating damage numbers. */
	void SetDamageNumberComponent(UDamageNumberComponent& InDamageNumberComponent);

	/** Set the widget used to display health and status effects. */
	void SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget);

	/** Set the widget used to display current attributes. */
	void SetAttributeDisplayWidget(UAttributeDisplayWidget& InAttributeDisplayWidget);

	/** Check if stunned. */
	UE_NODISCARD bool IsStunned() const;

	/**
	 * Apply damage.
	 * Used by USimpleEffect.
	 * 
	 * @param Applier					Attribute system of the character who did the damage.
	 * @param Damage					Amount of damage that will be dealt.
	 * @param bTriggerOnDamageEffects	Whether OnDamage effects should be triggered.
	 * @return							Amount of damage done, after damage taken modifiers are applied.
	 */
	double ApplyDamage(UAttributeSystemComponent* Applier, const double Damage, const bool bTriggerOnDamageEffects = false);

	/**
	 * Apply healing.
	 * Used by USimpleEffect.
	 * 
	 * @param HealAmount			Amount of health that will be healed.
	 * @param bTriggerOnHealEffects Whether OnHeal effects should be triggered.
	 * @return						Amount of health healed. 
	 *								Can be less than HealAmount if health was at or near maximum
	 */
	double ApplyHealing(const double HealAmount, const bool bTriggerOnHealEffects = false);

	/**
	 * Apply modifiers to the provided value.
	 * 
	 * @param BaseValue			Value before modifiers.
	 * @param ModifierToApply	Which modifier to use (damage or healing).
	 * @return					Value after modifiers.
	 */
	UE_NODISCARD double ApplyModifier(const double BaseValue, const EAttributeType ModifierToApply) const;

	/**
	 * Get the current bonus for the specified modifier.
	 * 
	 * @param ModifierToApply	Which modifier to use (damage or healing).
	 * @return					Bonus for the specified modifier.
	 */
	UE_NODISCARD double GetModifierBonus(const EAttributeType ModifierToApply) const;

	/**
	 * Get the current multiplier bonus for the specified modifier.
	 * 
	 * @param ModifierToApply	Which modifier to use (damage or healing).
	 * @return					Multiplier bonus for the specified modifier.
	 */
	UE_NODISCARD double GetModifierMultiplierBonus(const EAttributeType ModifierToApply) const;

	/**
	 * Check if the current health percent fulfills the comparison.
	 * 
	 * @param Comparand		Value to compare health percent to.
	 * @param Comparator	Flag representing the type of comparison to do (<,>,==).
	 * @return				True if the comparison is fulfilled, otherwise false.
	 */
	UE_NODISCARD bool DoesHealthPercentFulfillComparison(const double Comparand, const EConditionComparator Comparator) const;

	/**
	 * Alter the specified attribute.
	 * The type of the specified attribute info will determine which attribute is alterd.
	 * The bonus and multiplier bonuses will be added to the existing attribute info.
	 * Used by UAttributeAlteringEffect.
	 *
	 * @param AttributeInfo	Info about the attribute to alter.
	 */
	void AlterAttribute(const FAttribute& AttributeInfo);

	/** Apply a status effect. Used by UStatusEffect. */
	void ApplyStatusEffect(const UStatusEffectInternal& StatusEffect);

	/**
	 * Add a multi-stage bonus effect to the specified location.
	 * Used by UEffectAddingItemEffect.
	 * 
	 * @param Location		Which effects to modify.
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddEffectToLocation(const EAddEffectLocation Location, const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Remove a multi-stage bonus effect from the specified location.
	 * Used by UEffectAddingItemEffect.
	 * 
	 * @param Location		Which effects to modify.
	 * @param BonusEffectId Id of the bonus effects to remove.
	 */
	void RemoveEffectFromLocation(const EAddEffectLocation Location, const FEffectId BonusEffectId);

	/**
	 * Add a partial effect to basic attack effects.
	 * 
	 * @param PartialEffectInfo		Relevant info about the partial effect. @see FPartialEffectInfo
	 * @param InOutPartialEffectId	Id of the partial effect.
	 */
	void AddPartialEffectToBasicAttack(const FPartialEffectInfo& PartialEffectInfo, FEffectId& InOutPartialEffectId);

	/**
	 * Add a partial effect to registered ability effects.
	 * 
	 * @param PartialEffectInfo		Relevant info about the partial effect. @see FPartialEffectInfo
	 * @param InOutPartialEffectId	Id of the partial effect.
	 */
	void AddPartialEffectToAbilities(const FPartialEffectInfo& PartialEffectInfo, FEffectId& InOutPartialEffectId);

	/**
	 * Remove a partial effect from basic attack effects.
	 * 
	 * @param PartialEffectInfo	Relevant info about the partial effect. @see FPartialEffectInfo
	 * @param PartialEffectId	Id of the partial effect.
	 */
	void RemovePartialEffectFromBasicAttack(const FPartialEffectInfo& PartialEffectInfo, const FEffectId PartialEffectId);

	/**
	 * Remove a partial effect from registered ability effects.
	 * 
	 * @param PartialEffectInfo	Relevant info about the partial effect. @see FPartialEffectInfo
	 * @param PartialEffectId	Id of the partial effect.
	 */
	void RemovePartialEffectFromAbilities(const FPartialEffectInfo& PartialEffectInfo, const FEffectId PartialEffectId);

	/** Check if a status effect with the specified name exists on the status effect component. */
	UE_NODISCARD bool HasStatusEffect(const FName& StatusName) const;

	/** Get the current amount health. */
	UE_NODISCARD double GetCurrentHealth() const;

	/** Get the amount of missing health. */
	UE_NODISCARD double GetMissingHealth() const;

	/** Get the current percentage of health. */
	UE_NODISCARD double GetHealthPercent() const;

	/** Restore amount of health from saved value. */
	void RestoreSavedHealth(const double SavedHealth);

	/** 
	 * Get the number of stacks of the status effect with the specified name from
	 * the status effect component.
	 */
	UE_NODISCARD uint8 GetStatusEffectStacks(const FName& StatusName);

	/** Remove the status effect with the specified name from the status effect component. */
	UE_NODISCARD uint8 RemoveStatusEffects(const FName& StatusName);

	/**
	 * Push the owner.
	 * 
	 * @param Origin	Position to push from.
	 * @param Amount	Amount of force to be applied, will pull if negative.
	 */
	void Push(const FVector2D& Origin, const double Amount);

	/** Disable an equipped item by name. */
	void DisableItemByName(const FName& ItemName);

	/** Register an ability effect so that bonus effects can be added. */
	void RegisterAbilityEffect(UMultiStageEffect& Effect);

	/** Un-register an ability effect and remove any added bonus effects. */
	void UnRegisterAbilityEffect(UMultiStageEffect& Effect);

	/** Register a basic attack effect so that bonus effects can be added. */
	void RegisterBasicAttackEffect(UMultiStageEffect& Effect);

	/** Delegate called when the owner dies. */
	FSimpleDelegate OnDeathDelegate;

	/**
	 * Delegate called when stunned and when stun is removed.
	 * 
	 * @param bStunned Whether the character is currently stunned.
	 */
	FSetStunnedDelegate SetStunnedDelegate;

	/**
	 * Delegate called to get the current level of the owner.
	 * Must be bound.
	 * 
	 * @return Current level of the owner.
	 */
	FGetLevelDelegate GetLevelDelegate;

	/** Delegate called when modifiers are changed. */
	FSimpleMulticastDelegate OnModifiersChangedDelegate;

	/**
	 * Delegate called to disable an item with the specified name.
	 * 
	 * @param ItemName Name of the item to disable
	 */
	FDisableInventoryItemByNameDelegate DisableInventoryItemByNameDelegate;

private:
	/** Update base health value from a curve based on current level. */
	void UpdateBaseHealthFromCurve();

	/** Event called when the specified attribute is changed. */
	void OnAttributeChanged(const EAttributeType AttributeType);

	/**
	 * Update max health based on the health attribute's current value.
	 * @param bHeal	Whether to heal the difference when the new value is greater than the old.
	 */
	void UpdateMaxHealth(const bool bHeal = true);

	/** Update movement speed based on the movement speed attribute's current value. */
	void UpdateMovementSpeed();

	/** Apply OnKill effects to self after the owner kills an enemy. */
	void ApplyOnKillEffects();

	/**
	 * Add a multi-stage bonus effect to basic attack effects.
	 * 
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddEffectToBasicAttack(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Add a multi-stage bonus effect to registered ability effects.
	 *
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddEffectToAbilities(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Add a multi-stage bonus effect to OnTakeDamage effects.
	 *
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddOnTakeDamageEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Add a multi-stage bonus effect to the OnHeal effects.
	 *
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddOnHealEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Add a multi-stage bonus effect to OnDeath effects.
	 *
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddOnDeathEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Add a multi-stage bonus effect to OnKill effects
	 *
	 * @param Effect		Effect to add.
	 * @param InOutEffectId Id of the bonus effects added.
	 */
	void AddOnKillEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId);

	/**
	 * Remove a multi-stage bonus effect from basic attack effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveEffectFromBasicAttack(const FEffectId BonusEffectId);

	/**
	 * Remove a multi-stage bonus effect from registered ability effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveEffectFromAbilities(const FEffectId BonusEffectId);

	/**
	 * Remove a multi-stage bonus effect from OnTakeDamage effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveOnTakeDamageEffect(const FEffectId BonusEffectId);

	/**
	 * Remove a multi-stage bonus effect from OnHeal effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveOnHealEffect(const FEffectId BonusEffectId);

	/**
	 * Remove a multi-stage bonus effect from OnDeath effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveOnDeathEffect(const FEffectId BonusEffectId);

	/**
	 * Remove a multi-stage bonus effect from OnKill effects.
	 *
	 * @param Effect		Effect to remove.
	 * @param InOutEffectId Id of the bonus effects to remove.
	 */
	void RemoveOnKillEffect(const FEffectId BonusEffectId);

	/** Event called when the character dies. */
	void OnDeath();

	/**
	 * Add a partial effect to the specified ability effect.
	 * 
	 * @param AbilityEffect		Ability effect to add partial effect to.
	 * @param IdPair			Relevant ids for the partial effect.
	 * @param PartialEffectInfo Relevant info about the partial effect. @see FPartialEffectInfo
	 */
	static void AddPartialEffectToAbility(UMultiStageEffect& AbilityEffect, FPartialEffectIdPair& IdPair, const FPartialEffectInfo* PartialEffectInfo);

	/**
	 * Remove a partial effect from the specified ability effect.
	 *
	 * @param AbilityEffect		Ability effect to remove partial effect from.
	 * @param IdPair			Relevant ids for the partial effect.
	 * @param PartialEffectInfo Relevant info about the partial effect. @see FPartialEffectInfo
	 */
	static void RemovePartialEffectFromAbility(UMultiStageEffect& AbilityEffect, const FPartialEffectIdPair& IdPair, const FPartialEffectInfo* PartialEffectInfo);

	/** Struct containing current attribute values. @see FDefaultAttributes */
	UPROPERTY(VisibleAnywhere, Category = "Idk|Character", meta = (ShowOnlyInnerProperties))
	FDefaultAttributes Attributes;

	/** 
	 * Stunned when Stun >= 1.0. Allows multiple applications of stun to stack.
	 * Also allows stun to build up over multiple smaller applications.
	 */
	UPROPERTY(VisibleInstanceOnly)
	double Stun = 0.0;

	/** Curve that determines base health value based on current level. */
	UPROPERTY(EditDefaultsOnly, Category = "Required Bindings")
	TSoftObjectPtr<UCurveFloat> HealthCurve;

	/** Multiplies the value from HealthCurve to give the maximum health value before modifiers. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk|Character", meta = (ClampMin = "0.01"))
	double BaseHealthMultiplier = 1.0;

	/** Base movement speed, before modifiers. */
	UPROPERTY(EditDefaultsOnly, Category = "Idk|Character", meta = (ClampMin = "1.0"))
	double BaseMoveSpeed = 600.0;

	/** Component that manages the owner's health. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UIdkHealthComponent> HealthComponent;

	/** Component that manages status effects. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStatusEffectComponent> StatusEffectComponent;

	/** Character movement component. */
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	/** Registered ability effects. */
	TSparseArray<UMultiStageEffect*> RegisteredAbilityEffects;

	/** Combination of all bonus effects applied to abilities. Added to newly registered abilities. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageBonusEffect> AbilityBonusEffect;

	/** Maps partial effect Ids to info about the applied partial effect. @see FAppliedPartialEffectInfo */
	TMap<FEffectId, FAppliedPartialEffectInfo> AbilityPartialEffectsInfo;

	/** Set of Ids for all partial effects applied to abilites. */
	FEffectIdSet AbilityPartialEffectIdSet;

	/** Maps the Ids of partial effect applied to the basic attack to bonus effect Ids and use Ids. @see FPartialEffectIdPair */
	TMap<FEffectId, FPartialEffectIdPair> BasicAttackPartialEffectIdMap;

	/** Set of Ids for all partial effects applied to the basic attack. */
	FEffectIdSet BasicAttackPartialEffectIdSet;

	/** Registered basic attack effect. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageEffect> BasicAttackEffect;

	/** Effect applied when taking damage. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageEffect> OnTakeDamageEffect;

	/** Effect applied when being healed. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageEffect> OnHealEffect;

	/** Effect applied just before dying. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageEffect> OnDeathEffect;

	/** Effect applied when killing an enemy. */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UMultiStageEffect> OnKillEffect;

	/** Widget that displays current attribute values. */
	UPROPERTY()
	TObjectPtr<UAttributeDisplayWidget> AttributeDisplayWidget;

	/** Component used to spawn floating damage numbers. */
	TObjectPtr<UDamageNumberComponent> DamageNumberComponent;

	/** Whether BeginPlay() has been called for the component. */
	UPROPERTY()
	bool bBegunPlay = false;
		
};
