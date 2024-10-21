// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/IdkRandomStream.h"

#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <Math/RandomStream.h>
#include <Math/UnrealMathUtility.h>
#include <Misc/AssertionMacros.h>
#include <Templates/UniquePtr.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>

void UIdkRandomStream::Init(const bool bFullSeed)
{
	InitInternal(bFullSeed);
}

void UIdkRandomStream::InitFromSavedSeed(const int32 InSeed)
{
	InitInternal(true, true, InSeed);
}

void UIdkRandomStream::GenerateNewSeed()
{
	Seed = FMath::RandRange((int64)MinSeed, (int64)MaxSeed);
	SeedString = AlphanumericStringFromSeed();

	Rng->Initialize(Seed);
}

int32 UIdkRandomStream::GenerateSeed()
{
	return Rng->RandRange(MinGeneratedSeed, MaxGeneratedSeed);
}

int32 UIdkRandomStream::RandRange(const int32 Min, const int32 Max)
{
	check(Max - Min >= 0);

	return Rng->RandRange(Min, Max);
}

int32 UIdkRandomStream::GetInitialSeed() const
{
	return InitialSeed;
}

UIdkRandomStream& UIdkRandomStream::CreateRandomStream(UObject* Outer, const int32 Seed)
{
	UIdkRandomStream* NewRng = NewObject<UIdkRandomStream>(Outer);

	check(NewRng);

	NewRng->InitInternal(false, true, Seed);

	return *NewRng;
}

int32 UIdkRandomStream::GenerateSeed(const int32 Seed)
{
	return FRandomStream(Seed).RandRange(MinGeneratedSeed, MaxGeneratedSeed);
}

int32 UIdkRandomStream::RandRange(const int32 Seed, const int32 Min, const int32 Max)
{
	return FRandomStream(Seed).RandRange(Min, Max);
}

FString UIdkRandomStream::SeedToString(const int32 Seed, const bool bFullSeed)
{
	int32 SeedDigits;
	int32 MinSeed;

	if (bFullSeed)
	{
		SeedDigits = FullSeedDigits;
		MinSeed = MinFullSeed;
	}
	else
	{
		SeedDigits = GeneratedSeedDigits;
		MinSeed = MinGeneratedSeed;
	}

	return AlphanumericStringFromSeed(Seed, SeedDigits, MinSeed);
}

void UIdkRandomStream::InitInternal(const bool bFullSeed, const bool bExistingSeed, const int32 InSeed)
{
	if (bFullSeed)
	{
		SeedDigits = FullSeedDigits;
		MinSeed = MinFullSeed;
		MaxSeed = MaxFullSeed;
	}
	else
	{
		SeedDigits = GeneratedSeedDigits;
		MinSeed = MinGeneratedSeed;
		MaxSeed = MaxGeneratedSeed;
	}

	Rng = TUniquePtr<FRandomStream>(new FRandomStream);

	if (bExistingSeed)
	{
		Seed = InSeed;
		SeedString = AlphanumericStringFromSeed();

		Rng->Initialize(Seed);
	}
	else
	{
		// Generate initial seed
		GenerateNewSeed();
	}

	InitialSeed = Seed;
}

FString UIdkRandomStream::AlphanumericStringFromSeed() const
{
	return AlphanumericStringFromSeed(Seed, SeedDigits, MinSeed);
}

FString UIdkRandomStream::AlphanumericStringFromSeed(const int32 Seed, const int32 SeedDigits, const int32 MinSeed)
{
	FString String = TEXT("");
	int64 Dividend = (int64)Seed - MinSeed;
	int64 Quotient = 0;
	int64 Remainder = 0;

	for (int32 i = 0; i < SeedDigits; ++i)
	{
		Quotient = Dividend / SeedBase;
		Remainder = Dividend % SeedBase;

		String.AppendChar(CharFromBase36(Remainder));

		Dividend = Quotient;
	}

	return String;
}
