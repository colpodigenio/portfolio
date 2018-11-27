// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Structures/PlayerInfo.h"
#include "GameplayPlayerController.generated.h"

class ASpectatorPawn;

/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API AGameplayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Sets default values
	AGameplayPlayerController();
	
	// Tick event
	virtual void Tick(float DeltaSeconds) override;

	// Player info for network game
	UPROPERTY(Replicated, BlueprintReadWrite)
	FPlayerInfo PlayerInfo;

private:
	// If true player can see score sheet
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bScoreSheetIsVisible;
	void ShowScoreSheet();
	void HideScoreSheet();

protected:
	// Used to set input via player controller
	virtual void SetupInputComponent() override;
};

