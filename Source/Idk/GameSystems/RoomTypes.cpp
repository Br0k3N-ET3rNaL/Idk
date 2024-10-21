// Fill out your copyright notice in the Description page of Project Settings.

#include "Idk/GameSystems/RoomTypes.h"

#include <Containers/Array.h>
#include <GameplayTagContainer.h>
#include <NativeGameplayTags.h>

UE_DEFINE_GAMEPLAY_TAG(TAG_Room, "Room")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Spawn, "Room.Spawn")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Boss, "Room.Boss")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Arena, "Room.Arena")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Hall, "Room.Hall")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Arena_Combat, "Room.Arena.Combat")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Arena_Elite, "Room.Arena.Elite")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Arena_Horde, "Room.Arena.Horde")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Hall_Shop, "Room.Hall.Shop")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Hall_Rest, "Room.Hall.Rest")
UE_DEFINE_GAMEPLAY_TAG(TAG_Room_Hall_Curse, "Room.Hall.Curse")

void FRoomTypes::GetRoomTypes(TArray<FGameplayTag>& OutRoomTypes)
{
	OutRoomTypes.Add(TAG_Room_Arena_Combat);
	OutRoomTypes.Add(TAG_Room_Arena_Elite);
	OutRoomTypes.Add(TAG_Room_Arena_Horde);
	OutRoomTypes.Add(TAG_Room_Hall_Shop);
	OutRoomTypes.Add(TAG_Room_Hall_Rest);
	OutRoomTypes.Add(TAG_Room_Hall_Curse);
	OutRoomTypes.Add(TAG_Room_Boss);
}
