// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/Character/IdkCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/DamageNumberComponent.h"
#include "Idk/DataValidationHelper.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/UI/HealthAndStatusBarWidget.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Delegates/Delegate.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <HAL/Platform.h>
#include <Internationalization/Text.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

AIdkCharacter::AIdkCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AttributeSystemComponent = CreateDefaultSubobject<UAttributeSystemComponent>(TEXT("AttributeSystemComp"));
	DamageNumberComponent = CreateDefaultSubobject<UDamageNumberComponent>(TEXT("DamageNumberComp"));

	// Set up the capsule component used for collisions
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	const float HalfCapsuleHeight = 110.f;
	CapsuleComp->SetRelativeLocation(FVector(0.0, 0.0, HalfCapsuleHeight));
	CapsuleComp->InitCapsuleSize(40.f, HalfCapsuleHeight);
	CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComp->IgnoreActorWhenMoving(this, true);

	// Set up the damage number component
	DamageNumberComponent->SetupAttachment(CapsuleComp);
	DamageNumberComponent->SetRelativeLocation(FVector(0.0, 0.0, HalfCapsuleHeight - 30.0));

	// Set up the skeletal mesh component
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetupAttachment(CapsuleComp);
	MeshComp->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	MeshComp->SetRelativeLocation(FVector(0.0, 0.0, -HalfCapsuleHeight));
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComp->bReceivesDecals = false;

	// Set up the character movement component
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.0f;
	MoveComp->MaxAcceleration = 2400.0f;
	MoveComp->BrakingFrictionFactor = 1.0f;
	MoveComp->BrakingFriction = 6.0f;
	MoveComp->GroundFriction = 8.0f;
	MoveComp->BrakingDecelerationWalking = 1400.0f;
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->RotationRate = FRotator(0.0, 270.0, 0.0);

	// Set up the attribute system component
	AttributeSystemComponent->SetMovementComponent(*MoveComp);
	AttributeSystemComponent->OnDeathDelegate.BindUObject(this, &AIdkCharacter::OnDeath);
	AttributeSystemComponent->GetLevelDelegate.BindUObject(this, &AIdkCharacter::GetLevel);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
}

#if WITH_EDITOR
EDataValidationResult AIdkCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (DeathAnimation.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Death animation is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!BasicAttackChain.IsValid())
	{
		Context.AddError(FText::FromString(TEXT("Basic attack chain is invalid.")));

		Result = EDataValidationResult::Invalid;
	}

	FDataValidationContext BasicAttackContext;
	const FText BasicAttackPrefix = FText::FromString(TEXT("Basic attack is invalid: "));

	if (BasicAttackAbilityInfo.IsDataValid(BasicAttackContext) == EDataValidationResult::Invalid)
	{
		FDataValidationHelper::AddPrefixAndAppendIssues(BasicAttackPrefix, Context, BasicAttackContext);

		Result = EDataValidationResult::Invalid;
	}

	for (const FAbilityInfo* AbilityInfo : AbilityPool)
	{
		check(AbilityInfo);

		if (AbilityInfo->IsDataValid(Context) == EDataValidationResult::Invalid)
		{
			Result = EDataValidationResult::Invalid;
		}
	}

	if (!GetMesh()->GetSkeletalMeshAsset())
	{
		Context.AddError(FText::FromString(TEXT("Skeletal mesh is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (!GetMesh()->AnimClass)
	{
		Context.AddError(FText::FromString(TEXT("Animation blueprint is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AIdkCharacter::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	bBegunPlay = true;
#endif

	AnimInstance = CastChecked<UIdkAnimInstance>(GetMesh()->GetAnimInstance());

	check(!DeathAnimation.IsNull());

	// Bind event handlers for animation montages
	AnimInstance->OnMontageEnded.AddDynamic(this, &AIdkCharacter::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AIdkCharacter::OnPlayMontageNotifyBegin);
	AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &AIdkCharacter::OnPlayMontageNotifyEnd);
	AnimInstance->OnMontageBlendingOut.AddDynamic(this, &AIdkCharacter::OnMontageBlendingOut);

	AttributeSystemComponent->SetDamageNumberComponent(*DamageNumberComponent);
}

void AIdkCharacter::Destroyed()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	SetActorTickEnabled(false);

	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->StopAllMontages(0.f);
	}

	GetMesh()->ClearAnimScriptInstance();

	Super::Destroyed();
}

void AIdkCharacter::SetHealthAndStatusBarWidget(UHealthAndStatusBarWidget& HealthAndStatusBarWidget)
{
	AttributeSystemComponent->SetHealthAndStatusBarWidget(HealthAndStatusBarWidget);
}

double AIdkCharacter::GetSpawnZOffset() const
{
	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

UAttributeSystemComponent* AIdkCharacter::GetAttributeSystem() const
{
	return AttributeSystemComponent;
}

void AIdkCharacter::OnDeath()
{
	// No default implementation
}

void AIdkCharacter::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// No default implementation
}

void AIdkCharacter::OnPlayMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// No default implementation
}

void AIdkCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BasicAttackChain.GetCurrentAttack())
	{
		BasicAttackChain.ResetChain();
	}
}

void AIdkCharacter::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	// No default implementation
}
