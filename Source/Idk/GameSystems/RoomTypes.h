// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include <Containers/Array.h>
#include <NativeGameplayTags.h>

struct FGameplayTag;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Spawn)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Boss)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Arena)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Hall)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Arena_Combat)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Arena_Elite)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Arena_Horde)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Hall_Shop)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Hall_Rest)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Room_Hall_Curse)

struct FRoomTypes final
{
public:
	/** Get all room types. */
	static void GetRoomTypes(TArray<FGameplayTag>& OutRoomTypes);
};
