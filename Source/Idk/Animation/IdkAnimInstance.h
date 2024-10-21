// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation/AnimInstance.h>
#include <Animation/PoseSnapshot.h>
#include <Math/MathFwd.h>
#include <UObject/ObjectMacros.h>

#include "IdkAnimInstance.generated.h"

/** Animation instance with additional helper functions. */
UCLASS()
class UIdkAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/** Freeze the current pose. */
	void FreezeRagdollPose();

protected:
	/**
	 * Get the current direction of movement.
	 * 
	 * @param ForwardVector	Forward vector of the character. 
	 * @param Velocity		Current velocity of the character. 
	 * @return				The angle between the forward vector and the velocity, in degrees.
	 */
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName="Direction") double GetMovementDirection(const FVector& ForwardVector, const FVector& Velocity);

	/** Pose snapshot taken after ragdolling. */
	UPROPERTY(BlueprintReadOnly)
	FPoseSnapshot RagdollSnapshot;

	/** Whether to use the snapshotted ragdoll pose. */
	UPROPERTY(BlueprintReadOnly)
	bool bUseRagdollSnapshot = false;
};
