// Fill out your copyright notice in the Description page of Project Settings.


#include "Idk/GameSystems/Spawners/SpawnableEnemyClasses.h"

#include <HAL/Platform.h>
#include <Misc/AssertionMacros.h>
#include <Templates/SubclassOf.h>
#include <UObject/NameTypes.h>
#include <UObject/UnrealType.h>

bool FEnemyTypeInfo::operator<(const FEnemyTypeInfo& Other) const
{
	return SpawnCost < Other.SpawnCost;
}

int32 FEnemyTypeInfo::GetSpawnCost() const
{
	return SpawnCost;
}

EEnemyRole FEnemyTypeInfo::GetEnemyRole() const
{
	return EnemyType;
}

TSubclassOf<AIdkEnemyCharacter> FEnemyTypeInfo::GetEnemyClass() const
{
	return EnemyClass;
}

#if WITH_EDITOR
void USpawnableEnemyClasses::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (PropertyName.IsEqual(GET_MEMBER_NAME_CHECKED(USpawnableEnemyClasses, EnemyTypes)))
	{
		// Keep the array sorted
		EnemyTypes.StableSort();
	}
}
#endif
