// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FPSCharacter.h"


// Sets default values
APickup::APickup()
{
	// Creates Scene root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Creates Weapon
	PickupWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	PickupWeapon->SetupAttachment(SceneRoot);
	PickupWeapon->SetRelativeLocation(FVector(0.0f, -15.0f, 0.0f));
	PickupWeapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupWeapon->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupWeapon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Creates Mesh
	PickupSupply = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	PickupSupply->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSupply->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSupply->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSupply->SetupAttachment(RootComponent);

	// Creates point light
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	Light->SetAttenuationRadius(100.0f);
	Light->SetIntensity(500.0f);
	Light->SetCastShadows(false);

	// Creates rotate component
	RotateComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotateComponent"));

	// Default power up duration
	PowerupDuration = 10.0f;

	// Default number of ticks
	NumberOfTicks = 10;

	// Default time between ticks
	TimeBetweenTicks = 1.0f;

	// Default tick counter value
	TickCounter = 0;

	// Defaults

	DeltaHealth = 20.0f;
	DeltaArmor = 20.0f;
	RegeneratingHealthOnTick = 5.0f;
	PowerupUser = nullptr;
}


// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

}

void APickup::OnPickedup()
{
	switch (PickupType)
	{
	case EPickupType::SupplyPickup:
		OnSupplyPickup();
		break;
	case EPickupType::PowerupOnTime:
		OnPowerupOnTimeActivated();
		GetWorldTimerManager().SetTimer(PowerupDurationTimer, this, &APickup::OnPowerupOnTimeExpired, PowerupDuration, false);
		break;
	case EPickupType::PowerupOnTick:
		PowerupUser->bIsHealthRegenerating = true;
		TickTrigger();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("You should choose pick up class"))
	}
}

void APickup::TickTrigger()
{
	OnPowerupOnTick();

	TickCounter++;
	if (TickCounter >= NumberOfTicks)
	{
		PowerupUser->bIsPowerupActive = false;
		PowerupUser->bIsHealthRegenerating = false;
		PowerupUser = nullptr;
		Destroy();
		return;
	}

	GetWorldTimerManager().SetTimer(TickTimer, this, &APickup::TickTrigger, TimeBetweenTicks, false);

}
