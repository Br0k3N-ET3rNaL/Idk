// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Containers/UnrealString.h>
#include <HAL/Platform.h>
#include <intsafe.h>
#include <Templates/UniquePtr.h>
#include <UObject/Object.h>
#include <UObject/ObjectMacros.h>

#include "IdkRandomStream.generated.h"

struct FRandomStream;

/** Random number generator. */
UCLASS(NotBlueprintable)
class UIdkRandomStream final : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize the random number generator.
	 * 
	 * @param bFullSeed	Whether to use a 6 or 4 digit base36 number as the seed. 
	 */
	void Init(const bool bFullSeed = false);

	/**
	 * Initialize the random number generator from a saved seed.
	 * 
	 * @param InSeed	Seed used to initialize the random number generator. 
	 */
	void InitFromSavedSeed(const int32 InSeed);

	/** Generates a new seed. */
	void GenerateNewSeed();

	/** Generate a seed for another random number generator. */
	UE_NODISCARD int32 GenerateSeed();

	/**
	 * Generate a random number within the range.
	 * 
	 * @param Min	Minimum possible number to generate. 
	 * @param Max	Maximum possible number to generate.
	 * @return		The generated number. 
	 */
	UE_NODISCARD int32 RandRange(const int32 Min, const int32 Max);

	/** Get the seed that the random number generator was initialized with. */
	UE_NODISCARD int32 GetInitialSeed() const;

	/**
	 * Create a new random number generator instance.
	 * 
	 * @param Outer	Object used to create the random number generator. 
	 * @param Seed	Seed to initialize the random number generator with. 
	 * @return		The newly created random number generator.
	 */
	UE_NODISCARD static UIdkRandomStream& CreateRandomStream(UObject* Outer, const int32 Seed);

	/**
	 * Generate a seed for another random number generator.
	 * 
	 * @param InSeed	Seed used to generate the new seed. 
	 * @return			The generated seed.
	 */
	UE_NODISCARD static int32 GenerateSeed(const int32 Seed);

	/**
	 * Generate a random number within the range.
	 * 
	 * @param Seed	Seed used to generate the new seed. 
	 * @param Min	Minimum possible number to generate. 
	 * @param Max	Maximum possible number to generate.
	 * @return		The generated number. 
	 */
	UE_NODISCARD static int32 RandRange(const int32 Seed, const int32 Min, const int32 Max);

	/**
	 * Converts a seed to its alphanumeric representation.
	 * 
	 * @param Seed		Seed to convert.
	 * @param bFullSeed	Whether the seed uses a 6 or 4 digit base36 number.  
	 * @return			The alphanumeric representation of the seed.
	 */
	UE_NODISCARD static FString SeedToString(const int32 Seed, const bool bFullSeed = false);

private:
	/**
	 * Initialize the random number generator.
	 * 
	 * @param bFullSeed		Whether to use a full length seed. 
	 * @param bExistingSeed	Whether to initialize from an existing seed. 
	 * @param InSeed		Seed used to initialize the random number generator.
	 */
	void InitInternal(const bool bFullSeed, const bool bExistingSeed = false, const int32 InSeed = 0);

	/** Gets the alphanumeric representation of the random number generator's current seed. */
	UE_NODISCARD FString AlphanumericStringFromSeed() const;

	/**
	 * Converts a seed to its alphanumeric representation.
	 * 
	 * @param Seed			Seed to convert.
	 * @param SeedDigits	Number of base36 digits the seed uses.
	 * @param MinSeed		Minimum possible seed.
	 * @return				The alphanumeric representation of the seed.
	 */
	UE_NODISCARD static FString AlphanumericStringFromSeed(const int32 Seed, const int32 SeedDigits, const int32 MinSeed);

	/** Converts a single digit of a base36 number to the character representing it. */
	UE_NODISCARD static constexpr TCHAR CharFromBase36(const int32 Digit)
	{
		return (Digit >= 0 && Digit <= 9) ? Digit + '0'
			: (Digit >= 10 && Digit <= 35) ? (Digit - 10) + 'A'
			: ' ';
	}

	/** Alphanumeric representation of the seed. */
	UPROPERTY(VisibleInstanceOnly)
	FString SeedString;

	/** Current seed for the random number generator. */
	UPROPERTY(VisibleInstanceOnly)
	int32 Seed;

	/** Seed that the random number generator was initialized with. */
	UPROPERTY(VisibleInstanceOnly)
	int32 InitialSeed;

	/** Random number generator. */
	TUniquePtr<FRandomStream> Rng;

	/** Number of base36 digits to use for the seed. Either 6 or 4. */
	int32 SeedDigits = 0;

	/** Minimum seed. */
	int32 MinSeed = 0;

	/** Maximum seed. */
	int32 MaxSeed = 0;

	/** Base used for seed. */
	static const int32 SeedBase = 36;

	/** Number of base36 digits for a full seed. */
	static const int32 FullSeedDigits = 6;

	/** Number of base36 digits for a generated seed. */
	static const int32 GeneratedSeedDigits = 4;

	/** Minimum for full seed. */
	static const int32 MinFullSeed = INT32_MIN;

	/** Maximum for full seed. @note 2176782335 = 36^6 - 1 */
	static const int32 MaxFullSeed = 2176782335 + MinFullSeed;

	/** Minimum for generated seed. */
	static const int32 MinGeneratedSeed = 0;

	/** Maximum for generated seed. @note 1679615 = 36^4 - 1 */
	static const int32 MaxGeneratedSeed = 1679615;
};
