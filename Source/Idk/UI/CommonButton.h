// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CommonButtonBase.h>
#include <Containers/EnumAsByte.h>
#include <Internationalization/Text.h>
#include <Layout/Margin.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <Widgets/Layout/SScaleBox.h>

#include "CommonButton.generated.h"

enum EHorizontalAlignment;
enum EVerticalAlignment;
class UBorder;
class UBorderSlot;
class UCommonTextBlock;
class UScaleBox;
struct FPropertyChangedEvent;

/** Button with text. @see UCommonButtonBase */
UCLASS(Abstract)
class UCommonButton final : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface.
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;
	//~ End UUserWidget Interface

	/** Set the text to display on the button. */
	void SetText(const FText& InText);

private:
	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UCommonButtonBase Interface.
	virtual void NativeOnCurrentTextStyleChanged() override;
	//~ End UCommonButtonBase Interface

	/** Border around text. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border;

	/** Scales text. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> ScaleBox;

	/** Text on the button. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ButtonText;

	/** Padding around text. */
	UPROPERTY(EditAnywhere, Category = "Content", meta = (DisplayName = "Padding"))
	FMargin ButtonPadding;

	/** Horizontal alignment of the text. */
	UPROPERTY(EditAnywhere, Category = "Content")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	/** Vertical alignment of the text. */
	UPROPERTY(EditAnywhere, Category = "Content")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;

	/** Determines how the text is scaled. */
	UPROPERTY(EditAnywhere, Category = "Content")
	TEnumAsByte<EStretch::Type> Stretch;

	/** Text to display on the button. */
	UPROPERTY(EditAnywhere, Category = "Content", meta = (MultiLine))
	FText Text;
};
