// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayGameMode.h"
#include "GameplayPlayerController.h"
#include "portfolio_projectHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "GameplayGameState.h"

AGameplayGameMode::AGameplayGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = Aportfolio_projectHUD::StaticClass();

	// use our custom player controller class
	PlayerControllerClass = AGameplayPlayerController::StaticClass();

	// use our custom GameState class
	GameStateClass = AGameplayGameState::StaticClass();

}

