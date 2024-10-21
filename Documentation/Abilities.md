# Abilities

## Ability Info
- [`FAbilityInfo`](../Source/Idk/GameSystems/AbilityInfo.h)
- [`FAbilityInfoBuilder`](../Source/Idk/GameSystems/AbilityInfoBuilder.h)
- Information that represents an ability
- `ActivateAbilityDelegate` should be bound to the ability's implementation
- Descriptions can be decorated with tags to inline certain information
  - `{SelfDamage}`, `{TargetDamage}`, and `{CallbackDamage}` can be used to display the damage of effects in their respective stages
    - Value will reflect current damage modifiers
  - `{SelfHealing}`, `{TargetHealing}`, and `{CallbackHealing}` can be used to display the heal amount of effects in their respective stages
    - Value will reflect current healing modifiers
  - `{StatusEffect=StatusName}` can be used to display the description of a status effect, where `StatusName` is the name of the status effect
  - `{Duration}` can be used to display the duration of an ability
  - `{Interval}` can be used to display how frequently the ability hits

## Ability Components
- [`UAbilityComponent`](../Source/Idk/Character/Components/AbilityComponent.h)
- Component that manages an ability for a character
- Manages the [ability indicator component](#ability-indicator-component) and the [ability collision component](#ability-collision-component)

### Player Ability Component
- [`UPlayerAbilityComponent`](../Source/Idk/Character/Components/PlayerAbilityComponent.h)
- Ability component used by player characters
- Updates the [widget used to display the ability](../Source/Idk/UI/AbilityWidget.h)

### AI Ability Component
- [`UAIAbilityComponent`](../Source/Idk/Character/Components/AIAbilityComponent.h)
- Ability component used by AI characters
- Contains helper functions used by the AI's behavior tree

## Ability Indicator Component
- [`UAbilityIndicatorComponent`](../Source/Idk/Character/Components/AbilityIndicatorComponent.h)
- Displays an indicator representing the ability's area of effect

## Ability Collision Component
- [`UAbilityCollisionComponent`](../Source/Idk/Character/Components/AbilityCollisionComponent.h)
- Gets all characters in the ability's area of effect when it is activated
