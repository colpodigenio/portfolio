// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structures/PlayerInfo.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "PlayerSaveGame.generated.h"



/**
 * 
 */
UCLASS()
class PORTFOLIO_PROJECT_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, BlueprintReadWrite)
	FPlayerInfo SavePlayerInfo;
	
};
