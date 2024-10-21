// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/TempMessageWidget.h"

#include <Components/TextBlock.h>
#include <Engine/TimerHandle.h>
#include <Internationalization/Text.h>

void UTempMessageWidget::ShowMessage(const FText& InMessage, const float Duration)
{
	Message->SetText(InMessage);

	if (Duration == 0.f)
	{
		PlayAnimation(Fade);
	}
	else
	{
		FTimerHandle Timer;

		GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
			{
				PlayAnimation(Fade);
			}, Duration, false);
	}
	
}

void UTempMessageWidget::NativeOnInitialized()
{
	SetRenderOpacity(0.f);
}
