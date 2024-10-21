// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/OverheadWidgetComponent.h"

#include <Components/WidgetComponent.h>
#include <Math/MathFwd.h>
#include <Misc/AssertionMacros.h>
#include <UObject/NameTypes.h>
#include <UObject/UnrealType.h>

UOverheadWidgetComponent::UOverheadWidgetComponent()
{
	UpdateDrawSize();
	bReceivesDecals = false;

	SetGenerateOverlapEvents(false);
}

#if WITH_EDITOR
void UOverheadWidgetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UOverheadWidgetComponent, BaseDrawSize))
	|| PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(UOverheadWidgetComponent, UpscaleFactor)))
	{
		UpdateDrawSize();
	}
}
#endif

void UOverheadWidgetComponent::UpdateDrawSize()
{
	SetDrawSize(BaseDrawSize * UpscaleFactor);
	SetRelativeScale3D(FVector(1.0 / UpscaleFactor));
}
