

#include "Idk/GameSystems/Projectile.h"

#include "Idk/Character/IdkCharacter.h"
#include <Components/CapsuleComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SphereComponent.h>
#include <Delegates/Delegate.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/Actor.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Particles/ParticleSystemComponent.h>
#include <Templates/Casts.h>
#include <Templates/SubclassOf.h>

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	ParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComp"));
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));

	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(22.0);
	SphereComp->SetCollisionProfileName(TEXT("Custom"), false);

	FCollisionResponseContainer SphereCollisionProfile;
	SphereCollisionProfile.SetAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollisionProfile.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	SphereComp->SetCollisionResponseToChannels(SphereCollisionProfile);
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SphereComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	CapsuleComp->SetupAttachment(RootComponent);
	CapsuleComp->SetRelativeRotation(FQuat(FVector::RightVector, UE_DOUBLE_HALF_PI));
	CapsuleComp->SetRelativeLocation(FVector(-CapsuleComp->GetScaledCapsuleRadius(), 0.0, 0.0));
	CapsuleComp->SetCollisionProfileName(TEXT("Custom"), false);

	FCollisionResponseContainer CapsuleCollisionProfile;
	CapsuleCollisionProfile.SetAllChannels(ECollisionResponse::ECR_Ignore);
	CapsuleCollisionProfile.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CapsuleComp->SetCollisionResponseToChannels(CapsuleCollisionProfile);
	CapsuleComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	
	ParticleSystemComp->SetupAttachment(CapsuleComp);
	ParticleSystemComp->SetRelativeRotation(CapsuleComp->GetRelativeRotation() * -1.0);

	MoveComp->SetUpdatedComponent(SphereComp);
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->bInitialVelocityInLocalSpace = false;
}

void AProjectile::Init(const FVector& InitialVelocity, TSubclassOf<AIdkCharacter> InCollisionFilterClass, AActor* OwningActor)
{
	CollisionFilterClass = InCollisionFilterClass;

	CapsuleComp->IgnoreActorWhenMoving(OwningActor, true);

	MoveComp->Velocity = InitialVelocity;
}

void AProjectile::InitHoming(const FVector& InitialVelocity, TSubclassOf<AIdkCharacter> InCollisionFilterClass, AActor* OwningActor, AActor* Target)
{
	MoveComp->HomingTargetComponent = Target->GetRootComponent();

	Init(InitialVelocity, InCollisionFilterClass, OwningActor);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	check(HomingDelay < Lifetime);

	// Start the lifetime timer
	GetWorldTimerManager().SetTimer(LifetimeTimer, this, &AProjectile::DeactivateProjectile, Lifetime);

	if (MoveComp->bIsHomingProjectile && HomingDelay > 0.f && MoveComp->HomingAccelerationMagnitude > 0.f)
	{
		HomingAmount = MoveComp->HomingAccelerationMagnitude;
		MoveComp->HomingAccelerationMagnitude = 0.f;

		// Set the homing delay timer
		GetWorldTimerManager().SetTimer(HomingDelayTimer, this, &AProjectile::EnableHoming, HomingDelay);
	}
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(CollisionFilterClass))
	{
		OnCollisionDelegate.ExecuteIfBound(CastChecked<AIdkCharacter>(OtherActor)->GetAttributeSystem());

		GetWorldTimerManager().ClearTimer(LifetimeTimer);
		DeactivateProjectile();
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GetWorldTimerManager().ClearTimer(LifetimeTimer);
	DeactivateProjectile();
}

void AProjectile::DeactivateProjectile()
{
	if (ParticleSystemComp->IsActive())
	{
		ParticleSystemComp->DeactivateSystem();
		ParticleSystemComp->OnSystemFinished.RemoveDynamic(this, &AProjectile::DestroyProjectile);
		ParticleSystemComp->OnSystemFinished.AddDynamic(this, &AProjectile::DestroyProjectile);
	}
	else
	{
		Destroy();
	}
}

void AProjectile::DestroyProjectile(UParticleSystemComponent* PSystem)
{
	Destroy();
}

void AProjectile::EnableHoming()
{
	MoveComp->HomingAccelerationMagnitude = HomingAmount;
}
