// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/MeleeEnemyCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityComponent.h"
#include "Idk/Character/Components/AIAbilityComponent.h"
#include "Idk/Character/IdkPlayerCharacter.h"
#include "Idk/EffectSystem/EffectBuilders/MultiStageEffectBuilder.h"
#include "Idk/GameSystems/AbilityInfoBuilder.h"
#include <Animation/AnimNotifies/AnimNotify.h>
#include <Components/BoxComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Containers/Array.h>
#include <Delegates/Delegate.h>
#include <Engine/CollisionProfile.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <HAL/Platform.h>
#include <Idk/GameSystems/AbilityInfo.h>
#include <Internationalization/Text.h>
#include <Misc/AssertionMacros.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UnrealType.h>
#include <UObject/UObjectGlobals.h>

AMeleeEnemyCharacter::AMeleeEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);

	// Initialize the right sword hitbox
	RightSwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightSwordHitbox"));
	RightSwordHitbox->SetupAttachment(MeshComp, RightSwordHitboxSocket);

	RightSwordHitbox->SetGenerateOverlapEvents(true);
	RightSwordHitbox->IgnoreActorWhenMoving(this, true);
	RightSwordHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RightSwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::OnSwordOverlapBegin);

	// Initialize the left sword hitbox
	LeftSwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftSwordHitbox"));
	LeftSwordHitbox->SetupAttachment(MeshComp, LeftSwordHitboxSocket);

	LeftSwordHitbox->SetGenerateOverlapEvents(true);
	LeftSwordHitbox->IgnoreActorWhenMoving(this, true);
	LeftSwordHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LeftSwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::OnSwordOverlapBegin);

	// Set the basic attack info
	BasicAttackAbilityInfo = FAbilityInfoBuilder::Begin()
		.Init(TEXT("BasicAttack"), FAbilityTargetingInfo::InitCustom())
		.SetEffect(FMultiStageEffectBuilder::CreateBasicDamageEffectDefault(this, 10.0))
		.Complete();
}

#if WITH_EDITOR
EDataValidationResult AMeleeEnemyCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (RightSwordHitboxSocket.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("AMeleeEnemyCharacter: Right sword hitbox socket is not set.")));

		Result = EDataValidationResult::Invalid;
	}
	else if (!GetMesh()->DoesSocketExist(RightSwordHitboxSocket))
	{
		Context.AddError(FText::FromString(TEXT("AMeleeEnemyCharacter: Right sword hitbox socket does not exist on mesh.")));

		Result = EDataValidationResult::Invalid;
	}

	if (LeftSwordHitboxSocket.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("AMeleeEnemyCharacter: Left sword hitbox socket is not set.")));

		Result = EDataValidationResult::Invalid;
	}
	else if (!GetMesh()->DoesSocketExist(LeftSwordHitboxSocket))
	{
		Context.AddError(FText::FromString(TEXT("AMeleeEnemyCharacter: Left sword hitbox socket does not exist on mesh.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void AMeleeEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(AMeleeEnemyCharacter, RightSwordHitboxSocket)))
	{
		RightSwordHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, RightSwordHitboxSocket);
	}
	else if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(AMeleeEnemyCharacter, LeftSwordHitboxSocket)))
	{
		LeftSwordHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, LeftSwordHitboxSocket);
	}
}
#endif

void AMeleeEnemyCharacter::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyBegin(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(RightDamageWindow))
	{
		RightSwordHitbox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	}
	else if (NotifyName.IsEqual(LeftDamageWindow))
	{
		LeftSwordHitbox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	}
	else if (NotifyName.IsEqual(CanAttackAgain))
	{
		AbilityComps[BasicAttackAbilityIndex]->EndAbility();
	}
}

void AMeleeEnemyCharacter::OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnPlayMontageNotifyEnd(NotifyName, BranchingPointPayload);

	if (NotifyName.IsEqual(RightDamageWindow))
	{
		RightSwordHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

		HitActors.Empty();
	}
	else if (NotifyName.IsEqual(LeftDamageWindow))
	{
		LeftSwordHitbox->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

		HitActors.Empty();
	}
}

void AMeleeEnemyCharacter::BasicAttack()
{
	AbilityComps[BasicAttackAbilityIndex]->ApplyEffect();

	AnimInstance->Montage_Play(&BasicAttackChain.GetNextAttack());
}

void AMeleeEnemyCharacter::OnSwordOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AIdkPlayerCharacter>() && !HitActors.Contains(OtherActor))
	{
		HitActors.Add(OtherActor);

		AbilityComps[BasicAttackAbilityIndex]->ApplyEffectToTargetDeferred(
			CastChecked<AIdkPlayerCharacter>(OtherActor)->GetAttributeSystem());
	}
}

TArray<FName> AMeleeEnemyCharacter::GetSocketNames() const
{
	return GetMesh()->GetAllSocketNames();
}
