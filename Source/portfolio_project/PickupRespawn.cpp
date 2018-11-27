// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupRespawn.h"
#include "Components/SphereComponent.h"
#include "FPSCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pickup.h"
#include "Engine/World.h"
#include "Components/PointLightComponent.h"
#include "DeathmatchGameState.h"
#include "Components/SceneComponent.h"

// Sets default values
APickupRespawn::APickupRespawn()
{
	PrimaryActorTick.bCanEverTick = true;
	// Default pickup size
	PickupRespawnSize = 80.0f;

	// Creates scene component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	// Creates sphere component to overlap
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(PickupRespawnSize);
	

	// Creates decal component
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->DecalSize = FVector(2.0f, PickupRespawnSize, PickupRespawnSize);
	DecalComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	DecalComponent->SetupAttachment(RootComponent);

	// Default respawn cooldown
	RespawnCooldown = 5.0f;

	OverlappedCharacter = nullptr;

}

// Called when the game starts or when spawned
void APickupRespawn::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	ADeathmatchGameState* DeathmatchGameStateInstance = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (DeathmatchGameStateInstance)
	{
		DeathmatchGameStateInstance->OnMatchStarted.AddDynamic(this, &APickupRespawn::NotifyOnMatchStart);
	}
	RespawnPickup();
}

#if WITH_EDITOR
void APickupRespawn::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SphereComponent->SetSphereRadius(PickupRespawnSize);
	DecalComponent->DecalSize = FVector(32.0f, PickupRespawnSize, PickupRespawnSize);
}
#endif

void APickupRespawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick calling overlap event if actor is still overlapping it
	if (bIsSpawned)
	{
		NotifyActorBeginOverlap(OverlappedCharacter);
	}
}

void APickupRespawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	OverlappedCharacter = Cast<AFPSCharacter>(OtherActor);
	if (OverlappedCharacter)
	{
		if (SpawnedPickupInstance && bIsSpawned)
		{
			SetActorTickEnabled(true);
			if (SpawnedPickupInstance->PickupType == EPickupType::SupplyPickup)
			{
				bIsSpawned = false;
				SpawnedPickupInstance->OnPickedup();
				SpawnedPickupInstance->PickupSupply->SetVisibility(false, true);
				SpawnedPickupInstance->PickupWeapon->SetVisibility(false, true);
				SpawnedPickupInstance->Light->SetVisibility(false, true);
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
				GetWorldTimerManager().SetTimer(Respawn_Timer, this, &APickupRespawn::RespawnPickup, RespawnCooldown, false);
				return;
			}
			if ((SpawnedPickupInstance->PickupType == EPickupType::PowerupOnTime || SpawnedPickupInstance->PickupType == EPickupType::PowerupOnTick) && !OverlappedCharacter->bIsPowerupActive)
			{
				bIsSpawned = false;
				SpawnedPickupInstance->PowerupUser = OverlappedCharacter;
				SpawnedPickupInstance->PowerupUser->bIsPowerupActive = true;
				SpawnedPickupInstance->OnPickedup();
				SpawnedPickupInstance->PickupSupply->SetVisibility(false, true);
				SpawnedPickupInstance->Light->SetVisibility(false, true);
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
				GetWorldTimerManager().SetTimer(Respawn_Timer, this, &APickupRespawn::RespawnPickup, RespawnCooldown, false);
				return;
			}			
		}
	}
}

void APickupRespawn::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	SetActorTickEnabled(false);
	OverlappedCharacter = nullptr;

}

void APickupRespawn::RespawnPickup()
{
	FActorSpawnParameters PickupSpawnParameters;
	PickupSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnedPickupInstance = GetWorld()->SpawnActor<APickup>(PickupToSpawn, GetActorLocation() + FVector(0.0f, 0.0f, 50.0f), GetActorRotation(), PickupSpawnParameters);
	SpawnedPickupInstance->SetOwner(this);
	bIsSpawned = true;
}

void APickupRespawn::NotifyOnMatchStart()
{
	if (!bIsSpawned)
	{
		GetWorldTimerManager().ClearTimer(Respawn_Timer);
		RespawnPickup();
	}
}
