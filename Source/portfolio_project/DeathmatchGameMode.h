// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeathmatchGameMode.generated.h"

class AGameplayPlayerController;
class APlayerStart;

/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API ADeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ADeathmatchGameMode();

	virtual void Tick(float DeltaSeconds) override;

protected:

	int32 NumberOfFragsToEndGame;

	// Start match
	void StartMatch();

	// Start match countdown
	void StartMatchCountDown();

	// End match countdown
	void EndMatch();

	// End match countdown
	void EndMatchCountDown();

	// Timer before match starts
	FTimerHandle MatchStart_Timer;

	// Timer befor exit to main menu
	FTimerHandle MatchEnd_Timer;

	// Choose player start instance to spawn pawn
	APlayerStart* ChooseStartPoint();

	// All player starts existing on the map
	TArray<AActor*> AllPlayerStartPoints;

	// Exits to main menu 
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintProtected = "true"))
	void ExitToMainMenu();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Defines if count down before match has started
	bool bHasMatchStartCountDownStarted;

	// Defines if count down after match has started
	bool bHasMatchEndCountDownStarted;

public:
	// True if match has started
	bool bMatchHasStarted;

	// Is set in Host Menu by player that creates a match
	UPROPERTY(BlueprintReadWrite)
	int32 NumberOfPlayers;
	// Number of players logged in
	UPROPERTY(BlueprintReadOnly)
	int32 NumberOfConnectedPlayers;

	UPROPERTY(BlueprintReadOnly)
	TArray<AGameplayPlayerController*> AllPlayers;

};
