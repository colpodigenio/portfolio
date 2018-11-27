// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayPlayerController.h"
#include "portfolio_project.h"
#include "UnrealNetwork.h"



AGameplayPlayerController::AGameplayPlayerController()
{
	// Enables tick event
	PrimaryActorTick.bCanEverTick = true;

	bScoreSheetIsVisible = false;
}

void AGameplayPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GetPawn())
	{
		ChangeState(NAME_Spectating);
	}
}

void AGameplayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up gameplay key bindings
	check(InputComponent);

	// Bind toggle score sheet visibility
	InputComponent->BindAction("ToggleScoreSheet", IE_Pressed, this, &AGameplayPlayerController::ShowScoreSheet);
	InputComponent->BindAction("ToggleScoreSheet", IE_Released, this, &AGameplayPlayerController::HideScoreSheet);

}

void AGameplayPlayerController::ShowScoreSheet()
{
	bScoreSheetIsVisible = true;
}

void AGameplayPlayerController::HideScoreSheet()
{
	bScoreSheetIsVisible = false;
}

void AGameplayPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameplayPlayerController, PlayerInfo);
}