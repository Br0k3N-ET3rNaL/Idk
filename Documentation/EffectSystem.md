# Table of Contents
- [Table of Contents](#table-of-contents)
- [Effect System](#effect-system)
  - [Single Stage Effects](#single-stage-effects)
    - [Simple Effects](#simple-effects)
      - [Saving Values](#saving-values)
      - [Using Saved Values](#using-saved-values)
      - [Types:](#types)
    - [Status Effects](#status-effects)
      - [Unique Status Effects](#unique-status-effects)
      - [Blueprint-Based Status Effects](#blueprint-based-status-effects)
    - [Single Stage Effect Groups](#single-stage-effect-groups)
      - [Complex Effects](#complex-effects)
      - [Bonus Effects](#bonus-effects)
  - [Multistage Effects](#multistage-effects)
      - [Stages:](#stages)
    - [Multistage Bonus Effects](#multistage-bonus-effects)
  - [Item Effects](#item-effects)
    - [Attribute Altering Effects](#attribute-altering-effects)
    - [Effect-Adding Item Effects](#effect-adding-item-effects)
    - [Partial Effect-Adding Item Effects](#partial-effect-adding-item-effects)

# Effect System
- Effects interact directly with a character's [attribute system component](../Source/Idk/Character/Components/AttributeSystemComponent.h)
- Effects can be created dynamically in blueprints or using builders in C++

## Single Stage Effects
- Single target
- Instantaneous effect

### Simple Effects
- [`USimpleEffect`](../Source/Idk/EffectSystem/SimpleEffect.h)
- [`FSimpleEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h)

#### Saving Values
- Some types of simple effects can save a value
  - Setting `SaveId` to a non-zero value will save the effect's value when it is applied

#### Using Saved Values
- Some types of simple effects can use a value saved by another effect 
  - Adding a `FEffectId` and `FEffectVariableValueInfo` to `ValuesToUse` will allow the effect to add the value from another effect to its value.
    - See [FEffectVariableValueInfo](../Source/Idk/EffectSystem/SimpleEffect.h) for additional details.

#### Types:
- **Damage**
  - Deals damage to the target
    - `BaseValue` determines the amount of damage done before modifiers
    - Can use values saved by other effects ([see](#using-saved-values))
  - Amount of damage dealt can be saved ([see](#saving-values))
- **Healing**
  - Heals the target
    - `BaseValue` determines the amount of health healed before modifiers
    - Can use values saved by other effects ([see](#using-saved-values))
  - Amount of health healed can be saved ([see](#saving-values))
- **Get Missing Health**
  - Saves the amount of health the target is missing ([see](#saving-values))
- **Get Health Percentage**
  - Saves the current health percentage of the target ([see](#saving-values))
- **Get Status Effect Stacks**
  - Name of the status effect is specified using `NameForEffect`
  - Saves the number of stacks of the specified status effect ([see](#saving-values))
- **Remove Status Effect**
  - Name of the status effect is specified using `NameForEffect`
  - If `bRemoveAllStatusEffects` is true, it will instead remove all status effects on the target
  - The number of stacks removed can be saved ([see](#saving-values))
- **Push**
  - Pushes the target
    - `BaseValue` determines the amount of force applied
      - Negative values will pull instead
      - Values in the range +/-[1.0,4.0] produce reasonable amounts of force
- **Disable Item**
  - Disables the effects of an item in the target's inventory
  - Name of the item is specified using `NameForEffect`

### Status Effects
- [`UStatusEffect`](../Source/Idk/EffectSystem/StatusEffect.h)
  - Contains either a unique status effect or a blueprint-based status effect
- [`UStatusEffectInternal`](../Source/Idk/EffectSystem/StatusEffectInternal.h)
  - Base subclass for all status effects
- Remains on the target for a specified duration
  - If `Duration` is -1.0, then the status effect lasts forever
- Multiple stacks of the same status effect can be applied to the same target
- Status effect names should be unique
- If `bRefreshable` is true, when a new stack is applied, all existing stacks have their durations refreshed
- Made up of two types of effects:
  - Instantaneous effects that are applied to the target at regular intervals
    - Can be any single stage effects (`USingleStageEffect`)
  - Persistent effects that are applied and removed alongside the status effect
    - Can be any item effects (`UItemEffect`)
- Descriptions can be decorated with tags to inline certain information
  - `{Damage}` can be used to display the total damage
    - Value will reflect current damage modifiers
  - `{Healing}` can be used to display the total heal amount
    - Value will reflect current healing modifiers
  - `{Duration}` can be used to display the duration of the status effect
  - `{Interval}` can be used to display how frequently the status effet activates

#### Unique Status Effects
- [`FUniqueStatusEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/UniqueStatusEffectBuilder.h)
- Status effect that is only used in one place

#### Blueprint-Based Status Effects
- [`UStatusEffectBlueprintBase`](../Source/Idk/EffectSystem/StatusEffectBlueprintBase.h)
- [`FBlueprintBasedStatusEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/BlueprintBasedStatusEffectBuilder.h)
- Status effect that is used in more than one place
- Needs to be a blueprint subclassing `UStatusEffectBlueprintBase`
- Needs to be added to the status effect table (`Content/EffectSystem/DA_StatusEffectTable`) in the editor

### Single Stage Effect Groups
- [`USingleStageEffectGroup`](../Source/Idk/EffectSystem/SingleStageEffectGroup.h)
- Single stage effect that groups other single stage effects together

#### Complex Effects
- [`UComplexEffect`](../Source/Idk/EffectSystem/ComplexEffect.h)
- [`FComplexEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/ComplexEffectBuilder.h)
- Single stage effect group that only applies subeffects if the condition is true

#### Bonus Effects
- [`UBonusEffect`](../Source/Idk/EffectSystem/BonusEffect.h)
- [`FBonusEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/BonusEffectBuilder.h)
- Single stage effect group that can be added to or removed from a specific stage of a multistage effect
- Used by [multistage bonus effects](#multistage-bonus-effects) and [partial effect-adding item effects](#partial-effect-adding-item-effects)

## Multistage Effects
- [`UMultiStageEffect`](../Source/Idk/EffectSystem/MultiStageEffect.h)
- [`FMultiStageEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h)
- Effect that contains three stages of single stage effects
#### Stages:
  - **Self Effects**: Applied to the character who is applying the multistage effect
  - **Target Effects**: Applied to the target of the multistage effect
  - **Callback Effects**: Applied to the character who is applying the multistage effect after target effects have been applied
    - Only relevant for effects which use values from target effects ([see](#using-saved-values)), e.g., lifesteal

### Multistage Bonus Effects
- [`UMultiStageBonusEffect`](../Source/Idk/EffectSystem/MultiStageBonusEffect.h)
- [`FMultiStageBonusEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/MultiStageBonusEffectBuilder.h)
- Effect that contains three stages of bonus effects that can be added to or removed from a [multistage effect](#multistage-effects)
- Used by [effect-adding item effects](#effect-adding-item-effects)

## Item Effects
- [`UItemEffect`](../Source/Idk/EffectSystem/ItemEffect.h)
- Effect that remains on a target until removed
- Used by items and [status effects](#status-effects)

### Attribute Altering Effects
- [`UAttributeAlteringEffect`](../Source/Idk/EffectSystem/AttributeAlteringEffect.h)
- [`FAttributeAlteringEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h)
- Effect that alters a target's [attributes](../Source/Idk/GameSystems/IdkAttribute.h), e.g., health, movement speed, etc.

### Effect-Adding Item Effects
- [`UEffectAddingItemEffect`](../Source/Idk/EffectSystem/EffectAddingItemEffect.h)
- [`FEffectAddingItemEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/EffectAddingItemEffectBuilder.h)
- Effect that adds a [multistage bonus effect](#multistage-bonus-effects) to a specific [multistage effect](#multistage-effects)
- Possible locations that effects can be added to:
  - Basic attack effect
  - Ability effects
  - On take damage effects
    - Triggered when taking damage
  - On heal effects
    - Triggered when being healed
  - On death effects
    - Triggered when the character dies
  - On kill effects
    - Triggered when the character kills an enemy

### Partial Effect-Adding Item Effects
- [`UPartialEffectAddingItemEffect`](../Source/Idk/EffectSystem/PartialEffectAddingItemEffect.h)
- [`FPartialEffectAddingEffectBuilder`](../Source/Idk/EffectSystem/EffectBuilders/PartialEffectAddingEffectBuilder.h)
- Effect that adds a [bonus effect](#bonus-effects) that [uses the value](#using-saved-values) from all [simple effects](#simple-effects) of a specific [type](#types) in a specific [stage](#stages) of a specific [multistage effect](#multistage-effects)
- Possible locations that effects can be added to:
  - Basic attack effect
  - Ability effects
- Main use case is adding lifesteal
  - Add a heal effect that uses the value of damage effects to determine the amount of healing
