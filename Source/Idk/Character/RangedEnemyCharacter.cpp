// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/RangedEnemyCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AIAbilityComponent.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include <AIController.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Containers/Array.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <Engine/TimerHandle.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Math/MathFwd.h>
#include <Misc/DataValidation.h>
#include <Particles/ParticleSystemComponent.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

ARangedEnemyCharacter::ARangedEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom(1000.0, true, true))
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();
}

#if WITH_EDITOR
EDataValidationResult ARangedEnemyCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (BasicAttackBeam.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("ARangedEnemyCharacter: Basic attack beam is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (MuzzleSocket.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("ARangedEnemyCharacter: Muzzle socket is not set.")));

		Result = EDataValidationResult::Invalid;
	}
	else if (!GetMesh()->DoesSocketExist(MuzzleSocket))
	{
		Context.AddError(FText::FromString(TEXT("ARangedEnemyCharacter: Muzzle socket does not exist on mesh.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void ARangedEnemyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BasicAttackChain.GetCurrentAttack())
	{
		OnBasicAttackFireEnd();
	}

	Super::OnMontageEnded(Montage, bInterrupted);
}

void ARangedEnemyCharacter::BasicAttack()
{
	AbilityComps[BasicAttackAbilityIndex]->ApplyEffectToSelf();

	AAIController* AIController = CastChecked<AAIController>(GetController());
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	if (ChargeTime > 0.0)
	{
		FTimerHandle Timer;

		GetWorld()->GetTimerManager().SetTimer(Timer, this, &ARangedEnemyCharacter::OnBasicAttackChargeEnd, ChargeTime);
	}
	else
	{
		OnBasicAttackChargeEnd();
	}
}

void ARangedEnemyCharacter::OnBasicAttackChargeEnd()
{
	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());

	const USkeletalMeshComponent* MeshComp = GetMesh();
	const FTransform MuzzleTransform = MeshComp->GetSocketTransform(MuzzleSocket);

	FVector TargetLocation = AbilityComps[BasicAttackAbilityIndex]->GetInitialTargetLocation();

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleTransform.GetLocation(), TargetLocation, 
		ECollisionChannel::ECC_Pawn);

	if (AActor* Actor = HitResult.GetActor())
	{
		TargetLocation = Actor->GetActorLocation();

		if (Actor->IsA<AIdkPlayerCharacter>())
		{
			AbilityComps[BasicAttackAbilityIndex]->ApplyEffectToTargetDeferred(
				CastChecked<AIdkPlayerCharacter>(Actor)->GetAttributeSystem());
		}
	}

	// Spawn beam VFX
	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(), BasicAttackBeam.LoadSynchronous(),MuzzleTransform);

	// Connect the beam to the enemy's muzzle and the target
	Beam->SetBeamTargetPoint(0, TargetLocation, 0);
	Beam->SetBeamSourceTangent(0, MuzzleTransform.GetLocation() + MuzzleTransform.GetRotation().Vector(), 0);
}

void ARangedEnemyCharacter::OnBasicAttackFireEnd()
{
	if (IsValid(Player))
	{
		CastChecked<AAIController>(GetController())->SetFocus(Player, EAIFocusPriority::Gameplay);
	}
	
	AbilityComps[BasicAttackAbilityIndex]->EndAbility();
}

TArray<FName> ARangedEnemyCharacter::GetSocketNames() const
{
	return GetMesh()->GetAllSocketNames();
}

