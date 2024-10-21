// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/IdkPlayerCharacter.h"

#include "Idk/Animation/IdkAnimInstance.h"
#include "Idk/Character/Components/AbilityGeneratorComponent.h"
#include "Idk/Character/Components/AttributeSystemComponent.h"
#include "Idk/Character/Components/ExperienceComponent.h"
#include "Idk/Character/Components/GoldComponent.h"
#include "Idk/Character/Components/PlayerAbilityComponent.h"
#include "Idk/Character/Components/PlayerInventoryComponent.h"
#include "Idk/Character/IdkCharacter.h"
#include "Idk/Character/PlayerSaveData.h"
#include "Idk/EffectSystem/EffectBuilders/BlueprintBasedStatusEffectBuilder.h"
#include "Idk/EffectSystem/SingleStageEffect.h"
#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/Input/IdkInputConfig.h"
#include "Idk/Player/IdkPlayerController.h"
#include "Idk/UI/HUDWidget.h"
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <Components/WidgetInteractionComponent.h>
#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Engine/EngineBaseTypes.h>
#include <Engine/EngineTypes.h>
#include <EnhancedInputComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <HAL/Platform.h>
#include <InputActionValue.h>
#include <InputCoreTypes.h>
#include <InputTriggers.h>
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Misc/CoreMiscDefines.h>
#include <Misc/DataValidation.h>
#include <Templates/Casts.h>
#include <UObject/NameTypes.h>
#include <UObject/UObjectGlobals.h>

AIdkPlayerCharacter::AIdkPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bInputEnabled = true;

	// Create components
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	BasicAttackAbilityComp = CreateDefaultSubobject<UPlayerAbilityComponent>(TEXT("BasicAttackAbilityComp"));
	AbilityComp1 = CreateDefaultSubobject<UPlayerAbilityComponent>(TEXT("AbilityComp1"));
	AbilityComp2 = CreateDefaultSubobject<UPlayerAbilityComponent>(TEXT("AbilityComp2"));
	InventoryComp = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventoryComp"));
	ExperienceComp = CreateDefaultSubobject<UExperienceComponent>(TEXT("ExperienceComp"));
	GoldComp = CreateDefaultSubobject<UGoldComponent>(TEXT("GoldComp"));
	AbilityGeneratorComp = CreateDefaultSubobject<UAbilityGeneratorComponent>(TEXT("AbilityGeneratorComp"));

	// Attach ability components
	BasicAttackAbilityComp->SetupAttachment(RootComponent);
	AbilityComp1->SetupAttachment(RootComponent);
	AbilityComp2->SetupAttachment(RootComponent);

	// Initialize ability components
	BasicAttackAbilityComp->SetAttributeSystem(AttributeSystemComponent);
	AbilityComp1->SetAttributeSystem(AttributeSystemComponent);
	AbilityComp2->SetAttributeSystem(AttributeSystemComponent);

	// Set up the spring arm component
	SpringArmComp->SetupAttachment(GetCapsuleComponent());
	SpringArmComp->SetRelativeRotation(FRotator(-20.0, 0.0, 0.0));
	SpringArmComp->SetRelativeLocation(FVector(0.0, 0.0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->TargetArmLength = 300.0;

	// Set up the camera component
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->PostProcessSettings.bOverride_MotionBlurAmount = true;
	CameraComp->PostProcessSettings.MotionBlurAmount = 0.5;
	CameraComp->PostProcessSettings.bOverride_MotionBlurTargetFPS = true;
	CameraComp->PostProcessSettings.MotionBlurTargetFPS = 0.0;

	// Set up the widget interaction component
	WidgetInteractionComp->SetupAttachment(CameraComp);
	WidgetInteractionComp->InteractionSource = EWidgetInteractionSource::World;
	WidgetInteractionComp->InteractionDistance = 1000.0;

	// Set up the inventory component
	InventoryComp->SetAttributeSystem(AttributeSystemComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

#if WITH_EDITOR
EDataValidationResult AIdkPlayerCharacter::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult SuperResult = Super::IsDataValid(Context);
	EDataValidationResult Result = (SuperResult != EDataValidationResult::Invalid) ? EDataValidationResult::Valid : SuperResult;

	if (InputActions.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AIdkPlayerCharacter: Input actions is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (LevelUpVFX.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AIdkPlayerCharacter: Level up VFX is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	if (AbilityPool.Num() < 3)
	{
		Context.AddWarning(FText::FromString(TEXT("AIdkPlayerCharacter: Ability pool needs at least 3 abilities.")));

		Result = EDataValidationResult::Invalid;
	}

	if (LevelUpBonusClass.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("AIdkPlayerCharacter: Level up bonus class is not set.")));

		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AIdkPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetViewportMouseCaptureMode(GetWorld(), EMouseCaptureMode::CapturePermanently);

	AttributeSystemComponent->DisableInventoryItemByNameDelegate.BindUObject(this, &AIdkPlayerCharacter::DisableItem);

	ExperienceComp->OnLevelUpDelegate.BindUObject(this, &AIdkPlayerCharacter::OnLevelUp);

	check(!InputActions.IsNull());

	// Initialize the descriptions for all abilities
	BasicAttackAbilityInfo.InitDescription(*AttributeSystemComponent);

	for (FAbilityInfo* Ability : AbilityPool)
	{
		Ability->InitDescription(*AttributeSystemComponent);
	}

	// Set up the player's basic attack
	BasicAttackAbilityInfo.ActivateAbilityDelegate.BindUObject(this, &AIdkPlayerCharacter::BasicAttackImpl);
	BasicAttackAbilityComp->SetAbility(BasicAttackAbilityInfo);

	AttributeSystemComponent->RegisterBasicAttackEffect(BasicAttackAbilityInfo.GetEffect());

	LevelUpBonus = FBlueprintBasedStatusEffectBuilder::Begin(this)
		.Init(LevelUpBonusClass.LoadSynchronous())
		.CompleteWithValidation();

	LevelUpBonus->InitDescriptions(*AttributeSystemComponent);
}

void AIdkPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (!PlayerController)
	{
		PlayerController = CastChecked<AIdkPlayerController>(GetController());
	}

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	UIdkInputConfig* Actions = InputActions.LoadSynchronous();

	// Bind the actions
	PEI->BindAction(Actions->InputMove, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::Move);
	PEI->BindAction(Actions->InputLook, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::Look);
	PEI->BindAction(Actions->InputBasicAttack, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::BasicAttack);
	PEI->BindAction(Actions->InputAbility1, ETriggerEvent::Started, this, &AIdkPlayerCharacter::ShowAbility1Indicator);
	PEI->BindAction(Actions->InputAbility1, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::ActivateAbility1);
	PEI->BindAction(Actions->InputAbility2, ETriggerEvent::Started, this, &AIdkPlayerCharacter::ShowAbility2Indicator);
	PEI->BindAction(Actions->InputAbility2, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::ActivateAbility2);
	PEI->BindAction(Actions->ToggleInventory, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::ToggleInventory);
	PEI->BindAction(Actions->ToggleMenuMap, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::ToggleMenuMap);
	PEI->BindAction(Actions->EscapeKeyPressed, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::EscapeKeyPressed);
	PEI->BindAction(Actions->Interact, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::Interact);
	PEI->BindAction(Actions->Zoom, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::Zoom);
	PEI->BindAction(Actions->Pan, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::Pan);
	PEI->BindAction(Actions->ToggleMouse, ETriggerEvent::Triggered, this, &AIdkPlayerCharacter::ToggleMouse);
}

void AIdkPlayerCharacter::SetWidgets(UHealthAndStatusBarWidget& HealthAndStatusBar, FPlayerWidgets& PlayerWidgets)
{
	SetHealthAndStatusBarWidget(HealthAndStatusBar);

	check(PlayerWidgets.IsValid());

	AbilityComp1->SetAbilityWidget(*PlayerWidgets.AbilityWidget1);
	AbilityComp2->SetAbilityWidget(*PlayerWidgets.AbilityWidget2);

	InventoryComp->SetInventoryWidget(PlayerWidgets.InventoryWidget);

	ExperienceComp->SetExperienceWidget(*PlayerWidgets.ExperienceWidget);
	GoldComp->SetGoldWidget(*PlayerWidgets.GoldWidget);

	AbilityGeneratorComp->SetAbilityChoiceWidget(*PlayerWidgets.AbilityChoiceWidget);
	AbilityGeneratorComp->SetAbilitySwapWidget(*PlayerWidgets.AbilitySwapWidget);
	AbilityGeneratorComp->SetSaveData(SaveData);
	AbilityGeneratorComp->ToggleAbilityChoiceWidgetDelegate.BindLambda([&]()
		{
			PlayerController->GetHUDWidget().SwitchOpenMenu(EMenu::AbilityChoice, true);
		});
	AbilityGeneratorComp->ToggleAbilitySwapWidgetDelegate.BindLambda([&]()
		{
			PlayerController->GetHUDWidget().SwitchOpenMenu(EMenu::AbilitySwap, true);
		});
	AbilityGeneratorComp->AddAbilityToPlayerDelegate.BindUObject(this, &AIdkPlayerCharacter::AddAbility);

	AttributeSystemComponent->SetAttributeDisplayWidget(*PlayerWidgets.AttributeDisplayWidget);
}

void AIdkPlayerCharacter::InitMapWidget(const FMapInitInfo& MapInitInfo)
{
	PlayerController->GetHUDWidget().InitMapWidget(MapInitInfo);

	PlayerController->GetHUDWidget().UpdatePlayerPosition(FVector2D(GetActorLocation()));
}

void AIdkPlayerCharacter::InitSeed(const int32 Seed)
{
	AbilityGeneratorComp->Init(Seed, AbilityPool);
}

void AIdkPlayerCharacter::GenerateStartingAbility()
{
	AbilityGeneratorComp->GenerateAbilities(AbilityChoicesOnLevelUp, AbilityComp1->GetAbilityInfo(), AbilityComp2->GetAbilityInfo());
}

FVector AIdkPlayerCharacter::GetCameraForwardVector() const
{
	return CameraComp->GetForwardVector();
}

FVector AIdkPlayerCharacter::GetCameraLocation() const
{
	return CameraComp->GetComponentLocation();
}

FQuat AIdkPlayerCharacter::GetCameraRotation() const
{
	return CameraComp->GetComponentQuat();
}

void AIdkPlayerCharacter::SetInputEnabled(bool bInInputEnabled)
{
	bInputEnabled = bInInputEnabled;
}

void AIdkPlayerCharacter::AddItemToInventory(UItem& Item)
{
	InventoryComp->AddItem(Item);

	SaveData.AddItem(Item);
}

void AIdkPlayerCharacter::RestoreInventory(const TArray<UItem*>& Items)
{
	InventoryComp->RestoreInventory(Items);
}

void AIdkPlayerCharacter::GetItemTagCounts(TMap<FGameplayTag, int32>& OutItemTagCounts, int32& OutItemCount) const
{
	InventoryComp->GetItemTagCounts(OutItemTagCounts, OutItemCount);
}

void AIdkPlayerCharacter::AddExperience(const int32 Experience)
{
	ExperienceComp->AddExperience(Experience);
}

void AIdkPlayerCharacter::AddGold(const int32 Gold)
{
	GoldComp->AddGold(Gold);
}

bool AIdkPlayerCharacter::CanAfford(const int32 Price) const
{
	return GoldComp->GetGold() >= Price;
}

void AIdkPlayerCharacter::RemoveGold(const int32 Gold)
{
	check(CanAfford(Gold));

	GoldComp->RemoveGold(Gold);
}

void AIdkPlayerCharacter::OnBossKilled()
{
	PlayerController->GetHUDWidget().ShowGameEndScreen(true);
}

void AIdkPlayerCharacter::OnPossessed(AIdkPlayerController* InPlayerController)
{
	PlayerController = InPlayerController;

	OnPossessedDelegate.ExecuteIfBound();
}

const FPlayerSaveData& AIdkPlayerCharacter::GetSaveData() const
{
	// Update save data based on current values
	SaveData.SetHealth(AttributeSystemComponent->GetCurrentHealth());
	SaveData.SetLevelAndExperience(ExperienceComp->GetLevel(), ExperienceComp->GetExperience());
	SaveData.SetGold(GoldComp->GetGold());

	return SaveData;
}

void AIdkPlayerCharacter::RestoreSaveData(const FPlayerSaveData& InSaveData, const TArray<UItem*>& SavedItems)
{
	SaveData = InSaveData;

	AbilityGeneratorComp->SetSaveData(SaveData);
	AbilityGeneratorComp->RestoreSavedAbilities();

	ExperienceComp->RestoreLevelAndExperience(SaveData.GetLevel(), SaveData.GetExperience());

	GoldComp->SetGold(SaveData.GetGold());

	RestoreInventory(SavedItems);

	AttributeSystemComponent->RestoreSavedHealth(SaveData.GetHealth());
}

double AIdkPlayerCharacter::GetInteractionHeight()
{
	return InteractionHeight;
}

uint8 AIdkPlayerCharacter::GetLevel() const
{
	return ExperienceComp->GetLevel();
}

void AIdkPlayerCharacter::OnDeath()
{
	Super::OnDeath();

	// Disable collisions
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	BasicAttackAbilityComp->SetComponentTickEnabled(false);
	AbilityComp1->SetComponentTickEnabled(false);
	AbilityComp2->SetComponentTickEnabled(false);

	// Play death animation
	AnimInstance->Montage_Play(DeathAnimation.LoadSynchronous());

	PlayerController->GetHUDWidget().ShowGameEndScreen(false);
}

void AIdkPlayerCharacter::OnLevelUp(const uint8 NewLevel, const uint8 LevelsGained, const bool bRestoringFromSave)
{
	FApplyEffectParams Params = FApplyEffectParams(AttributeSystemComponent, AttributeSystemComponent);

	for (uint8 CurrentLevel = NewLevel - LevelsGained + 1; CurrentLevel <= NewLevel; ++CurrentLevel)
	{
		LevelUpBonus->ApplyEffect(Params);

		if (!bRestoringFromSave && CurrentLevel >= SecondAbilityLevel && (CurrentLevel - SecondAbilityLevel) % GainAbilityInterval == 0)
		{
			AbilityGeneratorComp->GenerateAbilities(AbilityChoicesOnLevelUp, AbilityComp1->GetAbilityInfo(), AbilityComp2->GetAbilityInfo());
		}
	}

	if (!bRestoringFromSave)
	{
		// Display level up VFX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LevelUpVFX.LoadSynchronous(), GetActorTransform());
	}
}

void AIdkPlayerCharacter::AddAbility(const int32 AbilityCompIndex, FAbilityInfo& NewAbility)
{
	check(AbilityCompIndex >= 0 && AbilityCompIndex <= 1);

	AttributeSystemComponent->RegisterAbilityEffect(NewAbility.GetEffect());

	TObjectPtr<UPlayerAbilityComponent>& AbilityComp = (AbilityCompIndex == 0) ? AbilityComp1 : AbilityComp2;

	AbilityComp->SetAbility(NewAbility);

	const int32 AbilityIndex = AbilityPool.IndexOfByKey(&NewAbility);

	check(AbilityIndex != INDEX_NONE);

	AbilityComps.FindOrAdd(AbilityIndex) = AbilityComp;
}

void AIdkPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (Controller && bInputEnabled && !AttributeSystemComponent->IsStunned())
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();

		const FVector Forward = GetActorForwardVector();
		const FVector Right = GetActorRightVector();
		const FVector Direction = UKismetMathLibrary::Multiply_VectorFloat(Forward, MoveValue.X) + 
			UKismetMathLibrary::Multiply_VectorFloat(Right, MoveValue.Y);

		AddMovementInput(Direction);

		PlayerController->GetHUDWidget().UpdatePlayerPosition(FVector2D(GetActorLocation()));
	}
}

void AIdkPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (Controller && !AttributeSystemComponent->IsStunned() && !PlayerController->bShowMouseCursor)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();
		
		if (LookValue.X != 0.0 && bInputEnabled)
		{
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.0)
		{
			FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
			SpringArmRotation.Pitch += LookValue.Y;
			SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch, -89.0, 20.0);
			SpringArmComp->SetRelativeRotation(SpringArmRotation);
		}
	}
}

void AIdkPlayerCharacter::ActivateAbility1(const FInputActionValue& Value)
{
	if (AbilityComp1->CanUseAbility() && bInputEnabled 
		&& !AttributeSystemComponent->IsStunned()
		&& !BasicAttackAbilityComp->IsUsingAbility())
	{
		AbilityComp1->StartAbility();
	}
	else if (AbilityComp1->IsIndicatorVisible())
	{
		AbilityComp1->HideIndicator();
	}
}

void AIdkPlayerCharacter::ActivateAbility2(const FInputActionValue& Value)
{
	if (AbilityComp2->CanUseAbility() && bInputEnabled 
		&& !AttributeSystemComponent->IsStunned()
		&& !BasicAttackAbilityComp->IsUsingAbility())
	{
		AbilityComp2->StartAbility();
	}
	else if (AbilityComp2->IsIndicatorVisible())
	{
		AbilityComp2->HideIndicator();
	}
}

void AIdkPlayerCharacter::ShowAbility1Indicator(const FInputActionValue& Value)
{
	if (AbilityComp1->CanShowIndicator()
		&& bInputEnabled && !AttributeSystemComponent->IsStunned()
		&& !BasicAttackAbilityComp->IsUsingAbility())
	{
		AbilityComp1->ShowIndicator();
	}
}

void AIdkPlayerCharacter::ShowAbility2Indicator(const FInputActionValue& Value)
{
	if (AbilityComp2->CanShowIndicator()
		&& bInputEnabled && !AttributeSystemComponent->IsStunned()
		&& !BasicAttackAbilityComp->IsUsingAbility())
	{
		AbilityComp2->ShowIndicator();
	}
}

void AIdkPlayerCharacter::Interact(const FInputActionValue& Value)
{
	// Simulates the player clicking on the hovered widget
	WidgetInteractionComp->PressPointerKey(EKeys::LeftMouseButton);
	WidgetInteractionComp->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AIdkPlayerCharacter::ToggleInventory(const FInputActionValue& Value)
{
	PlayerController->GetHUDWidget().SwitchOpenMenu(EMenu::Inventory, true);
}

void AIdkPlayerCharacter::ToggleMenuMap(const FInputActionValue& Value)
{
	PlayerController->GetHUDWidget().SwitchOpenMenu(EMenu::MenuMap, true);
}

void AIdkPlayerCharacter::EscapeKeyPressed(const FInputActionValue& Value)
{
	PlayerController->GetHUDWidget().OnEscapeKeyPressed();
}

void AIdkPlayerCharacter::Zoom(const FInputActionValue& Value)
{
	PlayerController->GetHUDWidget().UpdateMapZoom(Value.Get<FInputActionValue::Axis1D>());
}

void AIdkPlayerCharacter::Pan(const FInputActionValue& Value)
{
	PlayerController->GetHUDWidget().UpdateMapPan(Value.Get<FInputActionValue::Axis2D>());
}

void AIdkPlayerCharacter::BasicAttack(const FInputActionValue& Value)
{
	if (BasicAttackAbilityComp->CanUseAbility()
		&& bInputEnabled && !AttributeSystemComponent->IsStunned()
		&& !PlayerController->ShouldShowMouseCursor())
	{
		BasicAttackAbilityComp->StartAbility();
	}
}

void AIdkPlayerCharacter::ToggleMouse(const FInputActionValue& Value)
{
	PlayerController->ToggleMouseVisibility();
}

void AIdkPlayerCharacter::DisableItem(const FName& ItemName)
{
	InventoryComp->DisableItemByName(ItemName);
	SaveData.DisableItem(ItemName);
}
