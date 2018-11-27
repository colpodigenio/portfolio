// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerSaveGame.h"
#include "UnrealNetwork.h"


void UPlayerSaveGame::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerSaveGame, SavePlayerInfo);
}


