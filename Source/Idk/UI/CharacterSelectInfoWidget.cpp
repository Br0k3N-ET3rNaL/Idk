// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/CharacterSelectInfoWidget.h"

#include <Components/TextBlock.h>
#include <Internationalization/Text.h>

void UCharacterSelectInfoWidget::Init(const FText& InName, const FText& InDescription)
{
	Name->SetText(InName);
	Description->SetText(InDescription);
}
