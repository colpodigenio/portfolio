// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayPlayerState.h"
#include "UnrealNetwork.h"


void AGameplayPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameplayPlayerState, EnemiesKilled);
	DOREPLIFETIME(AGameplayPlayerState, PlayerNickName);
	DOREPLIFETIME(AGameplayPlayerState, PlayerPicture);
}
