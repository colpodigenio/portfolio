// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "portfolio_projectHUD.h"
#include "UObject/ConstructorHelpers.h"


ALobbyGameMode::ALobbyGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = Aportfolio_projectHUD::StaticClass();

	// use our custom player controller class
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
}