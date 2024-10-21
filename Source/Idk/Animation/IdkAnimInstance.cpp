// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Animation/IdkAnimInstance.h"

#include <Kismet/KismetMathLibrary.h>
#include <Math/MathFwd.h>

void UIdkAnimInstance::FreezeRagdollPose()
{
	SnapshotPose(RagdollSnapshot);

	bUseRagdollSnapshot = true;
}

double UIdkAnimInstance::GetMovementDirection(const FVector& ForwardVector, const FVector& Velocity)
{
	const FVector ForwardNorm = ForwardVector.GetSafeNormal();
	const FVector VelocityNorm = Velocity.GetSafeNormal();

	const FVector Cross = FVector::CrossProduct(ForwardNorm, VelocityNorm);
	const double Dot = FVector::DotProduct(ForwardNorm, VelocityNorm);
	double Direction = UKismetMathLibrary::DegAcos(Dot);

	if (Cross.Z <= 0.0)
	{
		Direction *= -1.0;
	}

	return Direction;
}
