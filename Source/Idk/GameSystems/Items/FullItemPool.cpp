// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Items/FullItemPool.h"

#include "Idk/Character/PlayerClasses.h"
#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/ConditionComparator.h"
#include "Idk/EffectSystem/Effect.h"
#include "Idk/EffectSystem/EffectBuilders/AttributeAlteringEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/BlueprintBasedStatusEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/BonusEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/ComplexEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/CompositeItemEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/EffectAddingItemEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageBonusEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/PartialEffectAddingEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SimpleEffectBuilder.h"
#include "Idk/EffectSystem/EffectBuilders/SingleStageEffectGroupBuilder.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/EffectStage.h"
#include "Idk/EffectSystem/SimpleEffect.h"
#include "Idk/EffectSystem/StatusEffectTable.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include "Idk/GameSystems/Items/Item.h"
#include "Idk/GameSystems/Items/ItemBuilder.h"
#include "Idk/UI/GenericDisplayInfo.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Math/Color.h>
#include <Misc/EnumRange.h>
#include <Templates/SubclassOf.h>
#include <Templates/UnrealTemplate.h>
#include <UObject/ConstructorHelpers.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

#if WITH_EDITOR
EDataValidationResult FDefaultItemArray::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	for (UItem* Item : Items)
	{
		if (Item->IsDataValid(Context) == EDataValidationResult::Invalid)
		{
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

FDefaultItemGroup::FDefaultItemGroup()
{
	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		Items.Add(Rarity);
	}
}

#if WITH_EDITOR
EDataValidationResult FDefaultItemGroup::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		if (Items[Rarity].IsDataValid(Context) == EDataValidationResult::Invalid)
		{
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

const FString UFullItemPool::StatusEffectTablePath = TEXT("/Game/Idk/EffectSystem/DA_StatusEffectTable");

UFullItemPool::UFullItemPool()
{
	StatusEffectTable = ConstructorHelpers::FObjectFinder<UStatusEffectTable>(*StatusEffectTablePath).Object;

	check(StatusEffectTable);

	for (const EPlayerClass PlayerClass : TEnumRange<EPlayerClass>())
	{
		ClassItems.Add(PlayerClass);
	}

	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		BackupItems.Add(Rarity);
	}

	RarityColors.Emplace(EItemRarity::Common, FLinearColor(1.0, 1.0, 1.0, 1.0));
	RarityColors.Emplace(EItemRarity::Uncommon, FLinearColor(0.0, 0.4, 0.0, 1.0));
	RarityColors.Emplace(EItemRarity::Rare, FLinearColor(0.0, 0.0, 0.6, 1.0));
	RarityColors.Emplace(EItemRarity::Legendary, FLinearColor(0.8, 0.2, 0.0, 1.0));
	RarityColors.Emplace(EItemRarity::Cursed, FLinearColor(0.5, 0.0, 0.0, 1.0));

	CreateBackupItems();
	CreateSharedItems();
	CreateKnightItems();
	CreateMageItems();

	StatusEffectTable = nullptr;
}

#if WITH_EDITOR
EDataValidationResult UFullItemPool::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	for (const EItemRarity Rarity : TEnumRange<EItemRarity>())
	{
		if (BackupItems[Rarity].IsDataValid(Context) == EDataValidationResult::Invalid)
		{
			Result = EDataValidationResult::Invalid;
		}
	}

	if (SharedItems.IsDataValid(Context) == EDataValidationResult::Invalid)
	{
		Result = EDataValidationResult::Invalid;
	}

	for (const EPlayerClass PlayerClass : TEnumRange<EPlayerClass>())
	{
		if (ClassItems[PlayerClass].IsDataValid(Context) == EDataValidationResult::Invalid)
		{
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

void UFullItemPool::CreateBackupItems()
{
	CreateCommonBackupItems();
	CreateUncommonBackupItems();
	CreateRareBackupItems();
	CreateLegendaryBackupItems();
	CreateCursedBackupItems();
}

void UFullItemPool::CreateCommonBackupItems()
{
	TArray<UItem*>& Items = BackupItems[EItemRarity::Common].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("HealthBoost1"))
		.Init(FGenericDisplayInfo(TEXT("Health Boost I"), TEXT("Gives +10 health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 10.0)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("SpeedBoost1"))
		.Init(FGenericDisplayInfo(TEXT("Speed Boost I"), TEXT("Gives +10% move speed.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::MovementSpeed, 0.0, 0.1)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Resistance1"))
		.Init(FGenericDisplayInfo(TEXT("Resistance I"), TEXT("Reduces damage taken by 10%.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::DamageTaken, 0.0, -0.1)
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateUncommonBackupItems()
{
	TArray<UItem*>& Items = BackupItems[EItemRarity::Uncommon].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("HealthBoost2"))
		.Init(FGenericDisplayInfo(TEXT("Health Boost II"), TEXT("Gives +20 health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 20.0)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("DamageBoost1"))
		.Init(FGenericDisplayInfo(TEXT("Damage Boost I"), TEXT("Gives +10% damage.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Damage, 0.0, 0.1)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("HealBoost1"))
		.Init(FGenericDisplayInfo(TEXT("Heal Boost I"), TEXT("Gives +30% healing.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Healing, 0.0, 0.3)
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateRareBackupItems()
{
	TArray<UItem*>& Items = BackupItems[EItemRarity::Rare].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("HealthBoost3"))
		.Init(FGenericDisplayInfo(TEXT("Health Boost III"), TEXT("Gives +50 health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 50.0)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("DefenseBoost1"))
		.Init(FGenericDisplayInfo(TEXT("Defense Boost I"), TEXT("Reduces damage taken by 10.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::DamageTaken, -10.0)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Vitality1"))
		.Init(FGenericDisplayInfo(TEXT("Vitality I"), TEXT("Gives +30% health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 0.0, 0.3)
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateLegendaryBackupItems()
{
	TArray<UItem*>& Items = BackupItems[EItemRarity::Legendary].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("HealthBoost4"))
		.Init(FGenericDisplayInfo(TEXT("Health Boost IV"), TEXT("Gives +100 health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 100.0)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Vitality2"))
		.Init(FGenericDisplayInfo(TEXT("Vitality II"), TEXT("Gives +50% health.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Health, 0.0, 0.5)
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("DamageBoost2"))
		.Init(FGenericDisplayInfo(TEXT("Damage Boost II"), TEXT("Gives +30% damage.")))
		.SetEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
			.Init(EAttributeType::Damage, 0.0, 0.3)
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateCursedBackupItems()
{
	TArray<UItem*>& Items = BackupItems[EItemRarity::Cursed].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("GlassCannon"))
		.Init(FGenericDisplayInfo(TEXT("Glass Cannon"), TEXT("-10% health, +10% damage.")))
		.SetEffect(FCompositeItemEffectBuilder::BeginDefault(this)
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::Health, 0.0, -0.1)
				.DeferCompletion())
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::Damage, 0.0, 0.2)
				.DeferCompletion())
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Turtle"))
		.Init(FGenericDisplayInfo(TEXT("Turtle"), TEXT("-10% move speed, -20% damage taken.")))
		.SetEffect(FCompositeItemEffectBuilder::BeginDefault(this)
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::MovementSpeed, 0.0, -0.1)
				.DeferCompletion())
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::DamageTaken, 0.0, -0.2)
				.DeferCompletion())
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("CursedVitality"))
		.Init(FGenericDisplayInfo(TEXT("Cursed Vitality"), TEXT("-10% health, +20% healing.")))
		.SetEffect(FCompositeItemEffectBuilder::BeginDefault(this)
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::Health, 0.0, -0.1)
				.DeferCompletion())
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::Healing, 0.0, 0.2)
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateSharedItems()
{
	CreateCommonSharedItems();
	CreateUncommonSharedItems();
	CreateRareSharedItems();
	CreateLegendarySharedItems();
	CreateCursedSharedItems();
}

void UFullItemPool::CreateCommonSharedItems()
{
	TArray<UItem*>& Items = SharedItems.Items[EItemRarity::Common].Items;
}

void UFullItemPool::CreateUncommonSharedItems()
{
	TArray<UItem*>& Items = SharedItems.Items[EItemRarity::Uncommon].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Reaper"))
	.Init(FGenericDisplayInfo(TEXT("Reaper"), TEXT("Heal for 10 when killing an enemy.")))
	.SetEffect(FCompositeItemEffectBuilder::BeginDefault(this)
		.AddEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::OnKill, FMultiStageBonusEffectBuilder::BeginDefault(this)
				.SetSelfEffect(FBonusEffectBuilder::BeginDefault(this)
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitHeal(10.0)
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.DeferCompletion())
	.Complete());
}

void UFullItemPool::CreateRareSharedItems()
{
	TArray<UItem*>& Items = SharedItems.Items[EItemRarity::Rare].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("AbilityLifesteal"))
		.Init(FGenericDisplayInfo(TEXT("Ability Lifesteal"), TEXT("Heal for 30% of the damage done by abilities to enemies.")))
		.SetEffect(FPartialEffectAddingEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::Abilities, EEffectStage::Callback, EEffectType::Damage)
			.SetEffect(FBonusEffectBuilder::BeginDefault(this)
				.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
					.InitHeal(0.0, false)
					.AddValueToUse(FEffectId::PartialEffectPlaceholderId, FEffectVariableValueInfo(0.3))
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("BasicAttackLifesteal"))
		.Init(FGenericDisplayInfo(TEXT("Basic Attack Lifesteal"), TEXT("Heal for 100% of damage done by basic attacks when under 30% health.")))
		.SetEffect(FPartialEffectAddingEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::BasicAttack, EEffectStage::Callback, EEffectType::Damage)
			.SetEffect(FBonusEffectBuilder::BeginDefault(this)
				.AddEffect(FComplexEffectBuilder::BeginDefault(this)
					.SetConditionToHealthPercentComparison(0.3, EConditionComparator::LessThan)
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitHeal(0.0, false)
						.AddValueToUse(FEffectId::PartialEffectPlaceholderId, FEffectVariableValueInfo(1.0))
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateLegendarySharedItems()
{
	TArray<UItem*>& Items = SharedItems.Items[EItemRarity::Legendary].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Resurrection"))
		.Init(FGenericDisplayInfo(TEXT("Resurrection"), TEXT("On death, resurrect with 30% health.")))
		.SetEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::OnDeath, FMultiStageBonusEffectBuilder::BeginDefault(this)
				.SetSelfEffect(FBonusEffectBuilder::BeginDefault(this)
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitGetMissingHealth(1)
						.DeferCompletion())
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitHeal(0.0, false)
						.AddValueToUse(1, FEffectVariableValueInfo(0.3, false, true))
						.DeferCompletion())
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitDisableItem(TEXT("Resurrection"))
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateCursedSharedItems()
{
	TArray<UItem*>& Items = SharedItems.Items[EItemRarity::Cursed].Items;
}

void UFullItemPool::CreateKnightItems()
{
	CreateCommonKnightItems();
	CreateUncommonKnightItems();
	CreateRareKnightItems();
	CreateLegendaryKnightItems();
	CreateCursedKnightItems();
}

void UFullItemPool::CreateCommonKnightItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Knight].Items[EItemRarity::Common].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("Thorns"))
		.Init(FGenericDisplayInfo(TEXT("Thorns"), TEXT("Deal 10 damage to attackers when taking damage.")))
		.SetEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::OnTakeDamage, FMultiStageBonusEffectBuilder::BeginDefault(this)
				.SetTargetEffect(FBonusEffectBuilder::BeginDefault(this)
					.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
						.InitDamage(10.0, false)
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateUncommonKnightItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Knight].Items[EItemRarity::Uncommon].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("DefensiveSurge"))
		.Init(FGenericDisplayInfo(TEXT("Defensive Surge"), TEXT("When using an ability, gain <StatusEffect id=\"Resilience\">Resilience</>.")))
		.SetEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::Abilities, FMultiStageBonusEffectBuilder::BeginDefault(this)
				.SetSelfEffect(FBonusEffectBuilder::BeginDefault(this)
					.AddEffect(FBlueprintBasedStatusEffectBuilder::BeginDefault(this)
						.Init(LoadStatusEffectByName(TEXT("Resilience")))
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateRareKnightItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Knight].Items[EItemRarity::Rare].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("AdaptiveSkin"))
		.Init(FGenericDisplayInfo(TEXT("Adaptive Skin"), TEXT("When taking damage, gain <StatusEffect id=\"Resilience\">Resilience</>.")))
		.SetEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
			.Init(EAddEffectLocation::OnTakeDamage, FMultiStageBonusEffectBuilder::BeginDefault(this)
				.SetSelfEffect(FBonusEffectBuilder::BeginDefault(this)
					.AddEffect(FBlueprintBasedStatusEffectBuilder::BeginDefault(this)
						.Init(LoadStatusEffectByName(TEXT("Resilience")))
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateLegendaryKnightItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Knight].Items[EItemRarity::Legendary].Items;
}

void UFullItemPool::CreateCursedKnightItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Knight].Items[EItemRarity::Cursed].Items;

	Items.Add(FItemBuilder::BeginDefault(this, TEXT("CursedThorns"))
		.Init(FGenericDisplayInfo(TEXT("Cursed Thorns"), TEXT("Reflect 100% of damage taken to attackers, but deal 20% reduced damage.")))
		.SetEffect(FCompositeItemEffectBuilder::BeginDefault(this)
			.AddEffect(FEffectAddingItemEffectBuilder::BeginDefault(this)
				.Init(EAddEffectLocation::OnTakeDamage, FMultiStageBonusEffectBuilder::BeginDefault(this)
					.SetTargetEffect(FBonusEffectBuilder::BeginDefault(this)
						.AddEffect(FSimpleEffectBuilder::BeginDefault(this)
							.InitDamage(0.0, false)
							.AddValueToUse(FEffectId::ConditionalEffectValueId, FEffectVariableValueInfo(1.0, false, true))
							.DeferCompletion())
						.DeferCompletion())
					.DeferCompletion())
				.DeferCompletion())
			.AddEffect(FAttributeAlteringEffectBuilder::BeginDefault(this)
				.Init(EAttributeType::Damage, 0.0, -0.2)
				.DeferCompletion())
			.DeferCompletion())
		.Complete());
}

void UFullItemPool::CreateMageItems()
{
	CreateCommonMageItems();
	CreateUncommonMageItems();
	CreateRareMageItems();
	CreateLegendaryMageItems();
	CreateCursedMageItems();
}

void UFullItemPool::CreateCommonMageItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Mage].Items[EItemRarity::Common].Items;
}

void UFullItemPool::CreateUncommonMageItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Mage].Items[EItemRarity::Uncommon].Items;
}

void UFullItemPool::CreateRareMageItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Mage].Items[EItemRarity::Rare].Items;
}

void UFullItemPool::CreateLegendaryMageItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Mage].Items[EItemRarity::Legendary].Items;
}

void UFullItemPool::CreateCursedMageItems()
{
	TArray<UItem*>& Items = ClassItems[EPlayerClass::Mage].Items[EItemRarity::Cursed].Items;
}

TSubclassOf<UStatusEffectBlueprintBase> UFullItemPool::LoadStatusEffectByName(FString&& StatusEffectName) const
{
	check(StatusEffectTable);

	return StatusEffectTable->GetStatusEffectClassByName(MoveTemp(StatusEffectName));
}
