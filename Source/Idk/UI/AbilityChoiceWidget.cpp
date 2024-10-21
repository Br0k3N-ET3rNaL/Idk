// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/UI/AbilityChoiceWidget.h"

#include "Idk/GameSystems/AbilityInfo.h"
#include "Idk/UI/GenericDisplayButtonWidget.h"
#include <Blueprint/UserWidget.h>
#include <Components/UniformGridPanel.h>
#include <Components/UniformGridSlot.h>
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Types/SlateEnums.h>

void UAbilityChoiceWidget::SetChoices(const TArray<const FAbilityInfo*>& AbilityChoices)
{
    check(!DisplayButtonWidgetClass.IsNull());

    AbilityGrid->ClearChildren();

    for (int32 i = 0; i < AbilityChoices.Num(); ++i)
    {
        UGenericDisplayButtonWidget* DisplayWidget
            = CreateWidget<UGenericDisplayButtonWidget>(this, DisplayButtonWidgetClass.LoadSynchronous());

        DisplayWidget->Init(AbilityChoices[i]->GetDisplayInfo());
        DisplayWidget->OnButtonClickedDelegate.BindLambda([&, i]()
            {
                AbilityChosenDelegate.Execute(i);
            });

        UUniformGridSlot* AbilityDisplaySlot = AbilityGrid->AddChildToUniformGrid(DisplayWidget, 0, i);

        AbilityDisplaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        AbilityDisplaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
    }
}
