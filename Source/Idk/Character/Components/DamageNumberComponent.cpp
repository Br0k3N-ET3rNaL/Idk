// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/Character/Components/DamageNumberComponent.h"

#include "Idk/IdkGameInstance.h"
#include <Containers/Array.h>
#include <HAL/Platform.h>
#include <Math/MathFwd.h>
#include <NiagaraDataInterfaceArrayFunctionLibrary.h>
#include <UObject/NameTypes.h>

const FName UDamageNumberComponent::DamageInfoArrayName = TEXT("DamageInfo");
const FName UDamageNumberComponent::IsHealArrayName = TEXT("IsHealArray");

void UDamageNumberComponent::AddDamageNumber(const double DamageDone)
{
	AddNumber(DamageDone, false);
}

void UDamageNumberComponent::AddHealNumber(const double AmountHealed)
{
	AddNumber(AmountHealed, true);
}

void UDamageNumberComponent::BeginPlay()
{
	Super::BeginPlay();

	SetAsset(&UIdkGameInstance::GetGameInstance(GetWorld())->GetDamageNumberParticleSystem());
}

void UDamageNumberComponent::AddNumber(const double Value, const bool bHeal)
{
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(this, DamageInfoArrayName);
	TArray<bool> IsHealArray = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayBool(this, IsHealArrayName);

	DamageList.Emplace(FVector4(GetComponentLocation(), Value));
	IsHealArray.Add(bHeal);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(this, DamageInfoArrayName, DamageList);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayBool(this, IsHealArrayName, IsHealArray);
}
