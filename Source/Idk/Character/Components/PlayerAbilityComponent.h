// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Idk/Character/Components/AbilityComponent.h"
#include <Engine/EngineBaseTypes.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>

#include "PlayerAbilityComponent.generated.h"

class AIdkCharacter;
class AIdkPlayerCharacter;
class UAbilityWidget;
class UAbilityIndicatorComponent;
struct FAbilityInfo;

/**
 * Ability component used by player characters.
 * Handles updating ability widgets.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UPlayerAbilityComponent final : public UAbilityComponent
{
	GENERATED_BODY()

public:
	UPlayerAbilityComponent();

	//~ Begin UActorComponent Interface.
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin UAbilityComponent public Interface.
	virtual void SetAbility(const FAbilityInfo& Ability) override;
	UE_NODISCARD virtual bool CanShowIndicator() const override;
	virtual void ShowIndicator() override;
	virtual void StartAbility() override;
	virtual void EndAbility() override;
	//~End UAbilityComponent public Interface

	/** Set ability widget to use. */
	void SetAbilityWidget(UAbilityWidget& InAbilityWidget);

	/** Check if the ability component can be activated. */
	UE_NODISCARD bool CanUseAbility() const;

	/** Check if the ability is currently active. */
	UE_NODISCARD bool IsUsingAbility() const;

private:
	//~ Begin UAbilityComponent protected Interface.
	UE_NODISCARD virtual FVector GetRangedTargetLocation() const override final;
	UE_NODISCARD virtual TSubclassOf<AIdkCharacter> GetCollisionClassFilter() const override final;
	UE_NODISCARD virtual FVector GetAimLocation() const override final;
	//~ End UAbilityComponent protected Interface

	/** Seconds remaining until cooldown has completed. */
	UPROPERTY(VisibleInstanceOnly)
	double CooldownRemaining = 0.0;

	/** Ability widget to use. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilityWidget> AbilityWidget;

	/** Length of the line trace to perform for GetAimLocation(). */
	UPROPERTY(VisibleInstanceOnly)
	double TraceDistance = 0.0;

	/** Player that owns this component. */
	TObjectPtr<AIdkPlayerCharacter> Player;

	/** Whether the ability is currently active. */
	bool bUsingAbility = false;

	/** Name of the collision profile to use for line traces in GetAimLocation(). */
	static const FName TraceCollisionProfile;

};
