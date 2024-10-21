# Items
- [`UItem`](../Source/Idk/GameSystems/Items/Item.h)
- [`FItemBuilder`](../Source/Idk/GameSystems/Items/ItemBuilder.h)
- Items can be created using the item builder
- For items to show up in a run, they need to be added to the item pool

## Item Pool
- [`UFullItemPool`](../Source/Idk/GameSystems/Items/FullItemPool.h)
- New items should be added in the constructor
  - `LoadStatusEffectByName()` can be used to get blueprint-based status effects from the status effect table
