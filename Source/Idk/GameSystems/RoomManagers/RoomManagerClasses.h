// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include <Templates/SubclassOf.h>
#include <UObject/ObjectMacros.h>

#include "RoomManagerClasses.generated.h"

class ABossRoomManager;
class ACombatRoomManager;
class ACurseRoomManager;
class AEliteRoomManager;
class AHordeRoomManager;
class ARestRoomManager;
class AShopRoomManager;
class ASpawnRoomManager;

/**
 *
 */
UCLASS()
class URoomManagerClasses : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<ASpawnRoomManager> Spawn;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<ABossRoomManager> Boss;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<ACombatRoomManager> Combat;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<AHordeRoomManager> Horde;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<AEliteRoomManager> Elite;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<AShopRoomManager> Shop;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<ARestRoomManager> Rest;

	UPROPERTY(EditDefaultsOnly, meta = (BlueprintBaseOnly))
	TSubclassOf<ACurseRoomManager> Curse;

};
