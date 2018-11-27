// Fill out your copyright notice in the Description page of Project Settings.

#define LOCTEXT_NAMESPACE "Your Namespace" 
#include "DeathmatchGameMode.h"
#include "GameplayPlayerController.h"
#include "DeathmatchGameState.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCharacter.h"
#include "Weapon.h"
#include "UnrealNetwork.h"
#include "GameplayPlayerState.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADeathmatchGameMode::ADeathmatchGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	bPauseable = true;
	bStartPlayersAsSpectators = true;
	PlayerControllerClass = AGameplayPlayerController::StaticClass();
	GameStateClass = ADeathmatchGameState::StaticClass();
	PlayerStateClass = AGameplayPlayerState::StaticClass();
	bMatchHasStarted = false;
	NumberOfConnectedPlayers = 0;
	bHasMatchStartCountDownStarted = false;
	bHasMatchEndCountDownStarted = false;
}

void ADeathmatchGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	StartMatch();
	EndMatch();	
}

void ADeathmatchGameMode::StartMatch()
{
	Cast<ADeathmatchGameState>(GetWorld()->GetGameState())->AllPlayers = AllPlayers;

	if (NumberOfConnectedPlayers == NumberOfPlayers && !bHasMatchStartCountDownStarted)
	{
		bHasMatchStartCountDownStarted = true;
		int32 PlayerCount = 0;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStartPoints);
		for (PlayerCount; PlayerCount < AllPlayers.Num(); ++PlayerCount)
		{
			int32 RandomStartPointIndex = FMath::RandRange(0.0f, (float)AllPlayerStartPoints.Num());
			AActor* StartPoint = AllPlayerStartPoints[RandomStartPointIndex];
			AllPlayerStartPoints.RemoveAt(RandomStartPointIndex);
			RestartPlayerAtPlayerStart(AllPlayers[PlayerCount], StartPoint);
		}

		GetWorldTimerManager().SetTimer(MatchStart_Timer, this, &ADeathmatchGameMode::StartMatchCountDown, 1.0f, true);
	}
}

void ADeathmatchGameMode::StartMatchCountDown()
{
	ADeathmatchGameState* DeathmatchGameStateInst = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (DeathmatchGameStateInst)
	{
		DeathmatchGameStateInst->MulticastOnSecondPassedBeforeMatchStart();
		if (DeathmatchGameStateInst->TimeBeforeMatchStart == 0)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStartPoints);
			for (auto It = AllPlayers.CreateConstIterator(); It; ++It)
			{
				AGameplayPlayerController* PlayerController = *It;

				bMatchHasStarted = true;
				AFPSCharacter* ControlledPawn = Cast<AFPSCharacter>(PlayerController->GetPawn());
				if (ControlledPawn)
				{
					ControlledPawn->DetachFromControllerPendingDestroy();
					ControlledPawn->bHUDCanBeDestroyed = true;
					ControlledPawn->CurrentWeapon->Destroy();
					DeathmatchGameStateInst->MulticastHideDestroyedPawn(ControlledPawn); // used to prevent accidental collision because of delay in destroying of the pawn
					ControlledPawn->SetLifeSpan(0.1f); // set life span is used instead of destroy to remove actor after removing of HUD to prevent bugs
				}
				int32 RandomStartPointIndex = FMath::RandRange(0.0f, (float)AllPlayerStartPoints.Num());
				AActor* StartPoint = AllPlayerStartPoints[RandomStartPointIndex];
				AllPlayerStartPoints.RemoveAt(RandomStartPointIndex);
				RestartPlayerAtPlayerStart(PlayerController, StartPoint);

				DeathmatchGameStateInst->MulticastBroadcastOnMatchStarted();
				DeathmatchGameStateInst->bMatchHasStarted = true;
				GetWorldTimerManager().ClearTimer(MatchStart_Timer);
			}
		}
	}
}

void ADeathmatchGameMode::EndMatch()
{
	ADeathmatchGameState* DeathmatchGameStateInst = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (DeathmatchGameStateInst && !bHasMatchEndCountDownStarted)
	{
		for (auto& PlayerStateInst : DeathmatchGameStateInst->PlayerArray)
		{
			AGameplayPlayerState* Player = Cast<AGameplayPlayerState>(PlayerStateInst);
			if (Player->EnemiesKilled == 3)
			{
				bHasMatchEndCountDownStarted = true;
				GetWorldTimerManager().SetTimer(MatchEnd_Timer, this, &ADeathmatchGameMode::EndMatchCountDown, 1.0f, true);
			}
		}
	}
}

void ADeathmatchGameMode::EndMatchCountDown()
{
	ADeathmatchGameState* DeathmatchGameStateInst = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (DeathmatchGameStateInst && DeathmatchGameStateInst->TimeBeforeExitToMainMenu > 0)
	{
		DeathmatchGameStateInst->MulticastOnSecondPassedBeforeExitToMainMenu();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(MatchEnd_Timer);
		ExitToMainMenu();
	}
}

APlayerStart* ADeathmatchGameMode::ChooseStartPoint()
{
	return nullptr;
}

void ADeathmatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	NumberOfConnectedPlayers++;
	AllPlayers.Emplace(Cast<AGameplayPlayerController>(NewPlayer));
}