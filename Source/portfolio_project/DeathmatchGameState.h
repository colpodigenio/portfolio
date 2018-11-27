// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DeathmatchGameState.generated.h"

class AGameplayPlayerController;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEndedSignature);

/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API ADeathmatchGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Sets default values
	ADeathmatchGameState();

	// Tick event
	virtual void Tick(float DeltaSeconds) override;

	// Runs every second when countdown before match starts
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastOnSecondPassedBeforeMatchStart();

	// Runs every second when countdown before match starts
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastOnSecondPassedBeforeExitToMainMenu();

	// Runs Restart() in game mode instance
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRespawn(AGameplayPlayerController* DeadPlayer);

	// Disable collision and hide mesh of destroyed pawn
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastHideDestroyedPawn(ACharacter* DestroyedPawn);

	// On match start delegate
	UPROPERTY()
	FOnMatchStartedSignature OnMatchStarted;

	// Broadcasts delegate on match start
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBroadcastOnMatchStarted();

	// Sort player array by enemies killed
	void SortPlayerArrayByEnemiesKilled();

	// On match end delegate
	UPROPERTY()
	FOnMatchEndedSignature OnMatchEnded;

	// Time before players can start match
	UPROPERTY(BlueprintReadOnly)
	int32 TimeBeforeMatchStart;

	// Time before players exit to main menu
	UPROPERTY(BlueprintReadOnly)
	int32 TimeBeforeExitToMainMenu;

	// Broadcasts delegate on match end
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBroadcastOnMatchEnded();

	// All players array
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<AGameplayPlayerController*> AllPlayers;

	// True if match has ended
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bMatchHasStarted;
	// True if match has ended
	UPROPERTY(BlueprintReadOnly)
	bool bMatchHasEnded;
};