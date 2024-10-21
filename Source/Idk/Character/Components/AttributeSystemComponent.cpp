// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/Components/AttributeSystemComponent.h"

#include "Idk/Character/Components/DamageNumberComponent.h"
#include "Idk/Character/Components/IdkHealthComponent.h"
#include "Idk/Character/Components/StatusEffectComponent.h"
#include "Idk/EffectSystem/AddEffectLocation.h"
#include "Idk/EffectSystem/ConditionComparator.h"
#include "Idk/EffectSystem/EffectId.h"
#include "Idk/EffectSystem/MultiStageBonusEffect.h"
#include "Idk/EffectSystem/MultiStageEffect.h"
#include "Idk/EffectSystem/SimpleEffect.h"
#include "Idk/GameSystems/IdkAttribute.h"
#include "Idk/UI/AttributeDisplayWidget.h"
#include "Idk/UI/HealthAndStatusBarWidget.h"
#include <Components/ActorComponent.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Containers/SparseArray.h>
#include <Curves/CurveFloat.h>
#include <Engine/EngineBaseTypes.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>

FDefaultAttributes::FDefaultAttributes()
{
	Attributes[Attributes.AddDefaulted()].Init(EAttributeType::Health, 100.0);
	Attributes[Attributes.AddDefaulted()].Init(EAttributeType::MovementSpeed, 600.0);
	Attributes[Attributes.AddDefaulted()].Init(EAttributeType::Damage);
	Attributes[Attributes.AddDefaulted()].Init(EAttributeType::Healing);
	Attributes[Attributes.AddDefaulted()].Init(EAttributeType::DamageTaken);

	for (int32 i = 0; i < Attributes.Num(); ++i)
	{
		AttributeToIndex.Add(Attributes[i].GetAttributeType(), i);
	}
}

FAttribute& FDefaultAttributes::operator[](const EAttributeType AttributeType)
{
	return Attributes[AttributeToIndex[AttributeType]];
}

const FAttribute& FDefaultAttributes::operator[](const EAttributeType AttributeType) const
{
	return Attributes[AttributeToIndex[AttributeType]];
}

bool FDefaultAttributes::Contains(const EAttributeType AttributeType) const
{
	return AttributeToIndex.Contains(AttributeType);
}

const TArray<FAttribute>& FDefaultAttributes::GetAttributes() const
{
	return Attributes;
}

UAttributeSystemComponent::UAttributeSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UIdkHealthComponent>(TEXT("HealthComponent"));
	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponent"));
}

#if WITH_EDITOR
EDataValidationResult UAttributeSystemComponent::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (HealthCurve.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("UAttributeSystemComponent: Health curve is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void UAttributeSystemComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UAttributeSystemComponent, HealthCurve)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UAttributeSystemComponent, BaseHealthMultiplier))
	{
		if (!HealthCurve.IsNull())
		{
			UpdateBaseHealthFromCurve();
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UAttributeSystemComponent, BaseMoveSpeed))
	{
		Attributes[EAttributeType::MovementSpeed].Update(BaseMoveSpeed);

		UpdateMovementSpeed();
	}
}
#endif

void UAttributeSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	bBegunPlay = true;

	StatusEffectComponent->SetAttributeSystem(*this);
}

void UAttributeSystemComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if (!HealthCurve.IsNull())
	{
		UpdateBaseHealthFromCurve();
	}
}

void UAttributeSystemComponent::InitBaseHealthMult(const double InBaseHealthMultiplier)
{
	BaseHealthMultiplier = InBaseHealthMultiplier;

	if (!HealthCurve.IsNull())
	{
		UpdateBaseHealthFromCurve();
	}
}

void UAttributeSystemComponent::InitBaseMoveSpeed(const double MoveSpeed)
{
	BaseMoveSpeed = MoveSpeed;

	Attributes[EAttributeType::MovementSpeed].Init(EAttributeType::MovementSpeed, BaseMoveSpeed);

	UpdateMovementSpeed();
}

void UAttributeSystemComponent::InitHealth()
{
	check(!HealthCurve.IsNull());

	UpdateBaseHealthFromCurve();
}

void UAttributeSystemComponent::SetHealthComponent(UIdkHealthComponent& InHealthComponent)
{
	HealthComponent = &InHealthComponent;
	UpdateMaxHealth();
}

void UAttributeSystemComponent::SetMovementComponent(UCharacterMovementComponent& InMovementComponent)
{
	MovementComponent = &InMovementComponent;
	UpdateMovementSpeed();
}

void UAttributeSystemComponent::SetStatusEffectComponent(UStatusEffectComponent& InStatusEffectComponent)
{
	StatusEffectComponent = &InStatusEffectComponent;
}

void UAttributeSystemComponent::SetDamageNumberComponent(UDamageNumberComponent& InDamageNumberComponent)
{
	DamageNumberComponent = &InDamageNumberComponent;
}

void UAttributeSystemComponent::SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget)
{
	check(StatusEffectComponent);

	HealthComponent->SetHealthBarWidget(HealthAndStatusBarWidget.GetHealthBarWidget());
	StatusEffectComponent->SetStatusEffectBarWidget(HealthAndStatusBarWidget.GetStatusEffectBarWidget());
}

void UAttributeSystemComponent::SetAttributeDisplayWidget(UAttributeDisplayWidget& InAttributeDisplayWidget)
{
	AttributeDisplayWidget = &InAttributeDisplayWidget;

	AttributeDisplayWidget->Init(Attributes.GetAttributes());
}

bool UAttributeSystemComponent::IsStunned() const
{
	return Stun >= 1.0;
}

double UAttributeSystemComponent::ApplyDamage(UAttributeSystemComponent* Applier, const double Damage, const bool bTriggerOnDamageEffects)
{
	// Apply the damage to the health component
	const double DamageDone = HealthComponent->TakeDamage(ApplyModifier(Damage, EAttributeType::DamageTaken));

	check(DamageNumberComponent);

	// Diplay damage number
	DamageNumberComponent->AddDamageNumber(DamageDone);

	// Check if character would die
	if (HealthComponent->GetHealth() == 0.0)
	{
		if (OnDeathEffect && OnDeathEffect->HasEffects())
		{
			// Apply OnDeath effects before character technically dies
			OnDeathEffect->ApplyEffectsToSelf(*this, 0.0);

			if (Applier)
			{
				OnDeathEffect->ApplyEffectsToTarget(*this, *Applier, 0.0);
			}
		}

		// Check if the character is still dead after applying OnDeath effects
		if (HealthComponent->GetHealth() == 0.0)
		{
			if (Applier)
			{
				Applier->ApplyOnKillEffects();
			}

			StatusEffectComponent->ClearStatusEffects();

			OnDeath();
		}
	}

	if (bTriggerOnDamageEffects && OnTakeDamageEffect && OnTakeDamageEffect->HasEffects())
	{
		OnTakeDamageEffect->ApplyEffectsToSelf(*this, DamageDone);

		if (Applier)
		{
			OnTakeDamageEffect->ApplyEffectsToTarget(*this, *Applier, DamageDone);
		}
	}

	return DamageDone;
}

double UAttributeSystemComponent::ApplyHealing(const double HealAmount, const bool bTriggerOnHealEffects)
{
	const double AmountHealed = HealthComponent->Heal(HealAmount);

	if (AmountHealed > 0.0)
	{
		check(DamageNumberComponent);

		DamageNumberComponent->AddHealNumber(HealAmount);
	}

	if (bTriggerOnHealEffects && OnHealEffect)
	{
		OnHealEffect->ApplyEffectsToSelf(*this, AmountHealed);
	}

	return AmountHealed;
}

double UAttributeSystemComponent::ApplyModifier(const double BaseValue, const EAttributeType ModifierToApply) const
{
	check(!FAttribute::CanAttributeHaveBaseValue(ModifierToApply));

	return Attributes[ModifierToApply].ApplyModifier(BaseValue);
}

double UAttributeSystemComponent::GetModifierBonus(const EAttributeType ModifierToApply) const
{
	check(!FAttribute::CanAttributeHaveBaseValue(ModifierToApply));

	return Attributes[ModifierToApply].GetBonus();
}

double UAttributeSystemComponent::GetModifierMultiplierBonus(const EAttributeType ModifierToApply) const
{
	check(!FAttribute::CanAttributeHaveBaseValue(ModifierToApply));

	return Attributes[ModifierToApply].GetMultiplierBonus();
}

bool UAttributeSystemComponent::DoesHealthPercentFulfillComparison(const double Comparand, const EConditionComparator Comparator) const
{
	const double HealthPercent = HealthComponent->GetHealthPercent();

	if ((Comparator & EConditionComparator::LessThan) != EConditionComparator::None && HealthPercent < Comparand)
	{
		return true;
	}

	if ((Comparator & EConditionComparator::Equal) != EConditionComparator::None && HealthPercent == Comparand)
	{
		return true;
	}

	if ((Comparator & EConditionComparator::GreaterThan) != EConditionComparator::None && HealthPercent > Comparand)
	{
		return true;
	}

	return false;
}

void UAttributeSystemComponent::AlterAttribute(const FAttribute& AttributeInfo)
{
	const EAttributeType AttributeToAlter = AttributeInfo.GetAttributeType();

	if (AttributeToAlter == EAttributeType::Stun)
	{
		Stun += AttributeInfo.GetBonus();
		SetStunnedDelegate.ExecuteIfBound(IsStunned());
	}
	else
	{
		check(Attributes.Contains(AttributeToAlter));

		Attributes[AttributeToAlter] += AttributeInfo;

		OnAttributeChanged(AttributeToAlter);

		if (AttributeDisplayWidget)
		{
			AttributeDisplayWidget->Update(Attributes[AttributeToAlter]);
		}
	}
}

void UAttributeSystemComponent::ApplyStatusEffect(const UStatusEffectInternal& StatusEffect)
{
	check(StatusEffectComponent);

	StatusEffectComponent->ApplyStatusEffect(StatusEffect);
}

void UAttributeSystemComponent::AddEffectToLocation(const EAddEffectLocation Location, const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	switch (Location)
	{
		case EAddEffectLocation::BasicAttack:
			AddEffectToBasicAttack(Effect, InOutEffectId);
			break;
		case EAddEffectLocation::Abilities:
			AddEffectToAbilities(Effect, InOutEffectId);
			break;
		case EAddEffectLocation::OnTakeDamage:
			AddOnTakeDamageEffect(Effect, InOutEffectId);
			break;
		case EAddEffectLocation::OnHeal:
			AddOnHealEffect(Effect, InOutEffectId);
			break;
		case EAddEffectLocation::OnDeath:
			AddOnDeathEffect(Effect, InOutEffectId);
			break;
		case EAddEffectLocation::OnKill:
			AddOnKillEffect(Effect, InOutEffectId);
			break;
		default:
			break;
	}
}

void UAttributeSystemComponent::RemoveEffectFromLocation(const EAddEffectLocation Location, const FEffectId BonusEffectId)
{
	switch (Location)
	{
		case EAddEffectLocation::BasicAttack:
			RemoveEffectFromBasicAttack(BonusEffectId);
			break;
		case EAddEffectLocation::Abilities:
			RemoveEffectFromAbilities(BonusEffectId);
			break;
		case EAddEffectLocation::OnTakeDamage:
			RemoveOnTakeDamageEffect(BonusEffectId);
			break;
		case EAddEffectLocation::OnHeal:
			RemoveOnHealEffect(BonusEffectId);
			break;
		case EAddEffectLocation::OnDeath:
			RemoveOnDeathEffect(BonusEffectId);
			break;
		case EAddEffectLocation::OnKill:
			RemoveOnKillEffect(BonusEffectId);
			break;
		default:
			break;
	}
}

void UAttributeSystemComponent::AddPartialEffectToBasicAttack(const FPartialEffectInfo& PartialEffectInfo, FEffectId& InOutPartialEffectId)
{
	if (InOutPartialEffectId == FEffectId::NoId)
	{
		// Generate an id for the partial effect
		InOutPartialEffectId = BasicAttackPartialEffectIdSet.GetNextValidId();
		BasicAttackPartialEffectIdSet.Add(InOutPartialEffectId);
		BasicAttackPartialEffectIdMap.Add(InOutPartialEffectId);
	}

	// Get the corresponding pair of ids
	FPartialEffectIdPair& IdPair = BasicAttackPartialEffectIdMap[InOutPartialEffectId];

	check(BasicAttackEffect);

	BasicAttackEffect->AddPartialEffect(PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);
}

void UAttributeSystemComponent::AddPartialEffectToAbilities(const FPartialEffectInfo& PartialEffectInfo, FEffectId& InOutPartialEffectId)
{
	const bool bWasSet = InOutPartialEffectId != FEffectId::NoId;

	if (!bWasSet)
	{
		// Generate an id for the partial effect
		InOutPartialEffectId = AbilityPartialEffectIdSet.GetNextValidId();
		AbilityPartialEffectIdSet.Add(InOutPartialEffectId);
		AbilityPartialEffectsInfo.Add(InOutPartialEffectId);
	}

	FAppliedPartialEffectInfo& AppliedPartialEffectInfo = AbilityPartialEffectsInfo[InOutPartialEffectId];

	if (bWasSet)
	{
		// Reapply partial effect to each ability effect
		for (auto AbilityEffectIt = RegisteredAbilityEffects.CreateIterator(); AbilityEffectIt; ++AbilityEffectIt)
		{
			// Get the mapped pair of ids for the current ability effect
			const FPartialEffectIdPair& IdPair = AppliedPartialEffectInfo.AbilityIdMap[AbilityEffectIt.GetIndex()];

			(*AbilityEffectIt)->AddPartialEffectStack(PartialEffectInfo, IdPair.BonusEffectId);
		}
	}
	else
	{
		AppliedPartialEffectInfo.ApplyPartialEffectDelegate.BindStatic(&UAttributeSystemComponent::AddPartialEffectToAbility, &PartialEffectInfo);
		AppliedPartialEffectInfo.RemovePartialEffectDelegate.BindStatic(&UAttributeSystemComponent::RemovePartialEffectFromAbility, &PartialEffectInfo);

		// Apply partial effect to each effect
		for (auto AbilityEffectIt = RegisteredAbilityEffects.CreateIterator(); AbilityEffectIt; ++AbilityEffectIt)
		{
			// Get the mapped pair of ids for the current ability effect
			FPartialEffectIdPair& IdPair = AppliedPartialEffectInfo.AbilityIdMap.FindOrAdd(AbilityEffectIt.GetIndex());

			(*AbilityEffectIt)->AddPartialEffect(PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);
		}
	}
}

void UAttributeSystemComponent::RemovePartialEffectFromBasicAttack(const FPartialEffectInfo& PartialEffectInfo, const FEffectId PartialEffectId)
{
	check(BasicAttackPartialEffectIdMap.Contains(PartialEffectId));
	const FPartialEffectIdPair& IdPair = BasicAttackPartialEffectIdMap[PartialEffectId];

	BasicAttackEffect->RemovePartialEffect(PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);

	BasicAttackPartialEffectIdMap.Remove(PartialEffectId);
	BasicAttackPartialEffectIdSet.Remove(PartialEffectId);
}

void UAttributeSystemComponent::RemovePartialEffectFromAbilities(const FPartialEffectInfo& PartialEffectInfo, const FEffectId PartialEffectId)
{
	check(AbilityPartialEffectsInfo.Contains(PartialEffectId));

	for (auto AbilityEffectIt = RegisteredAbilityEffects.CreateIterator(); AbilityEffectIt; ++AbilityEffectIt)
	{
		check(AbilityPartialEffectsInfo[PartialEffectId].AbilityIdMap.Contains(AbilityEffectIt.GetIndex()));

		const FPartialEffectIdPair& IdPair = AbilityPartialEffectsInfo[PartialEffectId].AbilityIdMap[AbilityEffectIt.GetIndex()];

		(*AbilityEffectIt)->RemovePartialEffect(PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);
	}

	AbilityPartialEffectsInfo.Remove(PartialEffectId);
	AbilityPartialEffectIdSet.Remove(PartialEffectId);
}

bool UAttributeSystemComponent::HasStatusEffect(const FName& StatusName) const
{
	check(StatusEffectComponent);

	return StatusEffectComponent->HasStatusEffect(StatusName);
}

double UAttributeSystemComponent::GetCurrentHealth() const
{
	return HealthComponent->GetHealth();
}

double UAttributeSystemComponent::GetMissingHealth() const
{
	return HealthComponent->GetMaxHealth() - HealthComponent->GetHealth();
}

double UAttributeSystemComponent::GetHealthPercent() const
{
	return HealthComponent->GetHealthPercent();
}

void UAttributeSystemComponent::RestoreSavedHealth(const double SavedHealth)
{
	HealthComponent->RestoreSavedHealth(SavedHealth);
}

uint8 UAttributeSystemComponent::GetStatusEffectStacks(const FName& StatusName)
{
	check(StatusEffectComponent);

	return StatusEffectComponent->GetStatusEffectStacks(StatusName);
}

uint8 UAttributeSystemComponent::RemoveStatusEffects(const FName& StatusName)
{
	check(StatusEffectComponent);

	if (StatusName.IsEqual(USimpleEffect::AnyStatusEffect))
	{
		return StatusEffectComponent->ClearStatusEffects();
	}
	else
	{
		return StatusEffectComponent->RemoveStatusEffect(StatusName);
	}
}

void UAttributeSystemComponent::Push(const FVector2D& Origin, const double Amount)
{
	ACharacter* OwningCharacter = CastChecked<ACharacter>(GetOwner());
	
	const FVector PushDirection = FVector(FVector2D(OwningCharacter->GetActorLocation()) - Origin, 0.0).GetSafeNormal();

	OwningCharacter->GetCharacterMovement()->AddForce(PushDirection * Amount);
}

void UAttributeSystemComponent::DisableItemByName(const FName& ItemName)
{
	check(DisableInventoryItemByNameDelegate.IsBound());

	DisableInventoryItemByNameDelegate.Execute(ItemName);
}

void UAttributeSystemComponent::RegisterAbilityEffect(UMultiStageEffect& Effect)
{
	if (AbilityBonusEffect && AbilityBonusEffect->HasEffects())
	{
		FEffectId EffectId = FEffectId::NoId;

		Effect.AddBonusEffects(*AbilityBonusEffect, EffectId);
	}

	int32 LowestIndex = 0;

	const int32 AbilityEffectIndex = RegisteredAbilityEffects.EmplaceAtLowestFreeIndex(LowestIndex);

	RegisteredAbilityEffects[AbilityEffectIndex] = &Effect;

	// Apply each applied partial effect to the newly registered ability effect
	for (TPair<FEffectId, FAppliedPartialEffectInfo>& Pair : AbilityPartialEffectsInfo)
	{
		FPartialEffectIdPair& IdPair = Pair.Value.AbilityIdMap.Add(AbilityEffectIndex);

		Pair.Value.ApplyPartialEffectDelegate.Execute(Effect, IdPair);
	}
}

void UAttributeSystemComponent::UnRegisterAbilityEffect(UMultiStageEffect& Effect)
{
	int32 AbilityEffectIndex = INDEX_NONE;

	// Get the index of the ability effect
	for (auto EffectIt = RegisteredAbilityEffects.CreateConstIterator(); EffectIt; ++EffectIt)
	{
		if ((*EffectIt) == &Effect)
		{
			AbilityEffectIndex = EffectIt.GetIndex();

			break;
		}
	}

	check(AbilityEffectIndex != INDEX_NONE);

	// Remove all partial effects
	for (TPair<FEffectId, FAppliedPartialEffectInfo>& Pair : AbilityPartialEffectsInfo)
	{
		Pair.Value.RemovePartialEffectDelegate.Execute(Effect, Pair.Value.AbilityIdMap[AbilityEffectIndex]);

		Pair.Value.AbilityIdMap.Remove(AbilityEffectIndex);
	}

	// Remove all bonus effects
	if (AbilityBonusEffect && AbilityBonusEffect->HasEffects())
	{
		RegisteredAbilityEffects[AbilityEffectIndex]->RemoveAllBonusEffects();
	}

	RegisteredAbilityEffects[AbilityEffectIndex] = nullptr;
}

void UAttributeSystemComponent::RegisterBasicAttackEffect(UMultiStageEffect& Effect)
{
	BasicAttackEffect = Effect;
}

void UAttributeSystemComponent::UpdateBaseHealthFromCurve()
{
	check(!HealthCurve.IsNull());
	check(GetLevelDelegate.IsBound());

	const double BaseBaseHealth = HealthCurve.LoadSynchronous()->GetFloatValue(GetLevelDelegate.Execute());

	const double NewBaseHealth = BaseHealthMultiplier * BaseBaseHealth;

	Attributes[EAttributeType::Health].Update(NewBaseHealth);

	UpdateMaxHealth();
}

void UAttributeSystemComponent::OnAttributeChanged(const EAttributeType AttributeType)
{
	switch (AttributeType)
	{
		case EAttributeType::Health:
			UpdateMaxHealth(false);
			break;
		case EAttributeType::MovementSpeed:
			UpdateMovementSpeed();
			break;
		case EAttributeType::Damage:
		case EAttributeType::Healing:
			OnModifiersChangedDelegate.Broadcast();
			break;
		default:
			break;
	}
}

void UAttributeSystemComponent::UpdateMaxHealth(const bool bHeal)
{
	HealthComponent->SetMaxHealth(Attributes[EAttributeType::Health].GetFinalValue(), bHeal);
}

void UAttributeSystemComponent::UpdateMovementSpeed()
{
	check(MovementComponent);

	MovementComponent->MaxWalkSpeed = Attributes[EAttributeType::MovementSpeed].GetFinalValue();
}

void UAttributeSystemComponent::ApplyOnKillEffects()
{
	if (OnKillEffect && OnKillEffect->HasEffects())
	{
		OnKillEffect->ApplyEffectsToSelf(*this, 0.0);
	}
}

void UAttributeSystemComponent::AddEffectToBasicAttack(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	check(BasicAttackEffect);

	BasicAttackEffect->AddBonusEffects(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);
}

void UAttributeSystemComponent::AddEffectToAbilities(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	if (!AbilityBonusEffect)
	{
		AbilityBonusEffect = NewObject<UMultiStageBonusEffect>(this);
	}

	const bool bNewId = AbilityBonusEffect->AddBonusEffect(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);

	for (UMultiStageEffect* AbilityEffect : RegisteredAbilityEffects)
	{
		AbilityEffect->AddBonusEffects(Effect, InOutEffectId, bNewId);
	}
}

void UAttributeSystemComponent::AddOnTakeDamageEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	if (!OnTakeDamageEffect)
	{
		OnTakeDamageEffect = NewObject<UMultiStageEffect>(this);
		OnTakeDamageEffect->AllowAddingTargetEffects();
	}

	OnTakeDamageEffect->AddBonusEffects(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);
}

void UAttributeSystemComponent::AddOnHealEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	if (!OnHealEffect)
	{
		OnHealEffect = NewObject<UMultiStageEffect>(this);
		OnHealEffect->AllowAddingTargetEffects();
	}

	OnHealEffect->AddBonusEffects(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);
}

void UAttributeSystemComponent::AddOnDeathEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	if (!OnDeathEffect)
	{
		OnDeathEffect = NewObject<UMultiStageEffect>(this);
		OnDeathEffect->AllowAddingTargetEffects();
	}

	OnDeathEffect->AddBonusEffects(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);
}

void UAttributeSystemComponent::AddOnKillEffect(const UMultiStageBonusEffect& Effect, FEffectId& InOutEffectId)
{
	if (!OnKillEffect)
	{
		OnKillEffect = NewObject<UMultiStageEffect>(this);
	}

	OnKillEffect->AddBonusEffects(Effect, InOutEffectId);

	check(InOutEffectId != FEffectId::NoId);
}

void UAttributeSystemComponent::RemoveEffectFromBasicAttack(const FEffectId BonusEffectId)
{
	BasicAttackEffect->RemoveBonusEffects(BonusEffectId);
}

void UAttributeSystemComponent::RemoveEffectFromAbilities(const FEffectId BonusEffectId)
{
	check(AbilityBonusEffect);

	AbilityBonusEffect->RemoveBonusEffects(BonusEffectId);

	for (UMultiStageEffect* AbilityEffect : RegisteredAbilityEffects)
	{
		AbilityEffect->RemoveBonusEffects(BonusEffectId);
	}
}

void UAttributeSystemComponent::RemoveOnTakeDamageEffect(const FEffectId BonusEffectId)
{
	check(OnTakeDamageEffect);

	OnTakeDamageEffect->RemoveBonusEffects(BonusEffectId);
}

void UAttributeSystemComponent::RemoveOnHealEffect(const FEffectId BonusEffectId)
{
	check(OnHealEffect);

	OnHealEffect->RemoveBonusEffects(BonusEffectId);
}

void UAttributeSystemComponent::RemoveOnDeathEffect(const FEffectId BonusEffectId)
{
	check(OnDeathEffect);

	OnDeathEffect->RemoveBonusEffects(BonusEffectId);
}

void UAttributeSystemComponent::RemoveOnKillEffect(const FEffectId BonusEffectId)
{
	check(OnKillEffect);

	OnKillEffect->RemoveBonusEffects(BonusEffectId);
}

void UAttributeSystemComponent::OnDeath()
{
	StatusEffectComponent->ClearStatusEffects();

	OnDeathDelegate.ExecuteIfBound();
}

void UAttributeSystemComponent::AddPartialEffectToAbility(UMultiStageEffect& AbilityEffect, FPartialEffectIdPair& IdPair, const FPartialEffectInfo* PartialEffectInfo)
{
	check(PartialEffectInfo);

	AbilityEffect.AddPartialEffect(*PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);
}

void UAttributeSystemComponent::RemovePartialEffectFromAbility(UMultiStageEffect& AbilityEffect, const FPartialEffectIdPair& IdPair, const FPartialEffectInfo* PartialEffectInfo)
{
	check(PartialEffectInfo);

	AbilityEffect.RemovePartialEffect(*PartialEffectInfo, IdPair.BonusEffectId, IdPair.SaveId);
}
