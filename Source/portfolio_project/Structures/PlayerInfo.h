// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define LOCTEXT_NAMESPACE "Your Namespace" 
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "PlayerInfo.generated.h"

/**
 Structure for storing player info data
 */

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

public:

	FPlayerInfo() {}
	//Name which player choose for his character
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FName PlayerName;
	//Player's avatar
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTexture2D* PlayerPicture;
	//Player's status whether he is ready or not
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FName PlayerStatus = TEXT("Not Ready");

};
