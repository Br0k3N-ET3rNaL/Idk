// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/EffectSystem/EffectId.h"

#include <HAL/Platform.h>

FEffectId::FEffectId(const uint8 Id)
	: Id(Id)
{
}

FEffectId::operator uint8() const
{
	return Id;
}

FEffectId& FEffectId::operator++()
{
	++Id;

	return *this;
}

FEffectId FEffectId::operator++(int)
{
	FEffectId Temp = *this;

	++Id;

	return Temp;
}

FEffectId& FEffectId::operator+=(const uint8 Rhs)
{
	Id += Rhs;

	return *this;
}
