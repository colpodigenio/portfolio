// Fill out your copyright notice in the Description page of Project Settings.

#include "DeathmatchGameState.h"
#include "GameplayPlayerController.h"
#include "DeathmatchGameMode.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "GameplayPlayerState.h"

ADeathmatchGameState::ADeathmatchGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bMatchHasStarted = false;
	bMatchHasEnded = false;
	TimeBeforeMatchStart = 5;
	TimeBeforeExitToMainMenu = 5;
}

void ADeathmatchGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

 	if (bMatchHasStarted && !bMatchHasEnded)
 	{
		SortPlayerArrayByEnemiesKilled();
	}
}

void ADeathmatchGameState::SortPlayerArrayByEnemiesKilled()
{
	int32 i = 0;
	for (i; i < PlayerArray.Num() - 1; ++i)
	{
		if (i + 1 <= PlayerArray.Num() - 1)
		{
			if (Cast<AGameplayPlayerState>(PlayerArray[i])->EnemiesKilled < Cast<AGameplayPlayerState>(PlayerArray[i + 1])->EnemiesKilled)
			{
				Swap(PlayerArray[i], PlayerArray[i + 1]);
			}
			else
			{
				continue;
			}
		}
	}
}

void ADeathmatchGameState::MulticastOnSecondPassedBeforeExitToMainMenu_Implementation()
{
	TimeBeforeExitToMainMenu--;
	if (!bMatchHasEnded)
	{
		bMatchHasEnded = true;
		for (auto& PlayerStateInst : PlayerArray)
		{
			AGameplayPlayerState* Player = Cast<AGameplayPlayerState>(PlayerStateInst);
			if (Player->EnemiesKilled == 3)
			{
				Player->GameResult = TEXT("You Won");
			}
		}
	}
}

bool ADeathmatchGameState::MulticastOnSecondPassedBeforeExitToMainMenu_Validate()
{
	return true;
}

void ADeathmatchGameState::MulticastBroadcastOnMatchStarted_Implementation()
{
	if (OnMatchStarted.IsBound())
	{
		OnMatchStarted.Broadcast();
	}
}

void ADeathmatchGameState::MulticastBroadcastOnMatchEnded_Implementation()
{
	if (OnMatchEnded.IsBound())
	{
		OnMatchEnded.Broadcast();
	}
}

void ADeathmatchGameState::MulticastHideDestroyedPawn_Implementation(ACharacter* DestroyedPawn)
{
	DestroyedPawn->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestroyedPawn->GetMesh()->SetVisibility(false);
}

bool ADeathmatchGameState::MulticastHideDestroyedPawn_Validate(ACharacter* DestroyedPawn)
{
	return true;
}

void ADeathmatchGameState::ServerRespawn_Implementation(AGameplayPlayerController* DeadPlayer)
{
	GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(DeadPlayer, GetWorld()->GetAuthGameMode()->ChoosePlayerStart(DeadPlayer));
}

bool ADeathmatchGameState::ServerRespawn_Validate(AGameplayPlayerController* DeadPlayer)
{
	return true;
}

void ADeathmatchGameState::MulticastOnSecondPassedBeforeMatchStart_Implementation()
{
	TimeBeforeMatchStart--;
}

bool ADeathmatchGameState::MulticastOnSecondPassedBeforeMatchStart_Validate()
{
	return true;
}

void ADeathmatchGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathmatchGameState, AllPlayers);
	DOREPLIFETIME(ADeathmatchGameState, bMatchHasStarted);
}
