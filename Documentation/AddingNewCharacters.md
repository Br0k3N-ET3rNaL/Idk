# Adding New Characters

## Adding New Player Characters
- Create a C++ subclass of [`AIdkPlayerCharacter`](../Source/Idk/Character/IdkPlayerCharacter.h)
  - Override `BasicAttackImpl()` with the implementation for the character's basic attack
  - For any abilities, initialize and add them to the ability pool in the constructor
    - Bind `ActivateAbilityDelegate` to the ability's implementation in `BeginPlay()`
  - See [`AKnightPlayerCharacter`](../Source/Idk/Character/KnightPlayerCharacter.h) or [`AMagePlayerCharacter`](../Source/Idk/Character/MagePlayerCharacter.h) for reference
- Add new items to the [item pool](../Source/Idk/GameSystems/Items/FullItemPool.h) ([see](./Items.md))
- Create a blueprint subclass from the C++ class
  - Check under "Required Bindings" for variables that need to be set
- Add a new value to the [player classes enum](../Source/Idk/Character/PlayerClasses.h)
- Add a new [`ACharacterSelectDisplayActor](../Source/Idk/CharacterSelectDisplayActor.h) to the character select level
  - Call `Init Preview Transform` for the new display actor in the character select level blueprint
  - Set the name and description for the new class in the character select level blueprint under "Idk/Character Info"

## Adding New Enemy Characters
- Create a C++ subclass of the appropriate type
  - [`ABossEnemyCharacter`](../Source/Idk/Character/BossEnemyCharacter.h) for bosses
  - [`AEliteEnemyCharacter`](../Source/Idk/Character/EliteEnemyCharacter.h) for elite enemies
  - [`AIdkEnemyCharacter`](../Source/Idk/Character/IdkEnemyCharacter.h) for basic enemies
- Override `BasicAttack()` with the implementation for the character's basic attack
- For any abilities, initialize and add them to the ability pool in the constructor
  - Bind `ActivateAbilityDelegate` to the ability's implementation in `BindAbilities()`
- Create a blueprint subclass from the C++ class
  - Check under "Required Bindings" for variables that need to be set
- Add the enemy to the appropriate data asset in the editor
  - `Content/GameSystems/DA_BossEncounters` for bosses
  - `Content/GameSystems/DA_EliteEncounters` for elite enemies
  - `Content/GameSystems/DA_SpawnableEnemyClasses` for basic enemies
