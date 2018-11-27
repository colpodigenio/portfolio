// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayGameState.h"




void AGameplayGameState::MulticastSomeFunction_Implementation()
{
	//It is short for iterator
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PlayerConrtollerInstance = It->Get();
		if(PlayerConrtollerInstance && PlayerConrtollerInstance->IsLocalController())
		{
			return;
		}
	}
}
