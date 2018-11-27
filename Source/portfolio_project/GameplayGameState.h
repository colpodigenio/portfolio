// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayGameState.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API AGameplayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSomeFunction();


};
