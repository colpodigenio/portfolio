// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define LOCTEXT_NAMESPACE "Your Namespace" 
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameplayPlayerState.generated.h"

class UTexture2D;

/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API AGameplayPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// Amount of enemies killed by player
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 EnemiesKilled = 0;

	// Player name that was set in player options
	UPROPERTY(Replicated, BlueprintReadWrite)
	FName PlayerNickName;
	
	// Player game state defines if the game won or lost
	UPROPERTY(BlueprintReadOnly)
	FName GameResult = TEXT("You Lost");

	//Player's avatar
	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	UTexture2D* PlayerPicture;
};
