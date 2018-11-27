// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupRespawn.generated.h"

class USphereComponent;
class UDecalComponent;
class APickup;
class AFPSCharacter;
class USceneComponent;

UCLASS()
class PORTFOLIO_PROJECT_API APickupRespawn : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	APickupRespawn();
	// Override Tick event
	virtual void Tick(float DeltaTime) override;

	// Override overlap events
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
#if WITH_EDITOR
	// Override post initialize method
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Creates scene component to be root
	UPROPERTY(VisibleAnywhere, Category = "Pickup", meta = (BlueprintProtected = "true"))
	USceneComponent* SceneComponent;

	// Creates sphere component
	UPROPERTY(VisibleAnywhere, Category = "Pickup", meta = (BlueprintProtected = "true"))
	USphereComponent* SphereComponent;

	// Creates a decal component
	UPROPERTY(VisibleAnywhere, Category = "Pickup", meta = (BlueprintProtected = "true"))
	UDecalComponent* DecalComponent;

	// Creates a pickup to spawn
	UPROPERTY(EditInstanceOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	TSubclassOf<APickup> PickupToSpawn;

	APickup* SpawnedPickupInstance;

	// Size of pickup
	UPROPERTY(EditDefaultsOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	float PickupRespawnSize;

	// Respawns pickup
	void RespawnPickup();

	// Pickup respawn cooldown
	UPROPERTY(EditAnywhere, Category = "Pickup", meta = (BlueprintProtected = "true"))
	float RespawnCooldown;

	// Respawn timer
	FTimerHandle Respawn_Timer;

	// Defines if pick up is already taken by player
	bool bIsSpawned;

	// Sound on pickup taken
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (BlueprintProtected = "true"))
	USoundBase* PickupSound;

	// Event called every time match starts
	UFUNCTION()
	void NotifyOnMatchStart();


public:
	// Instance of overlapped actor
	UPROPERTY(BlueprintReadOnly)
	AFPSCharacter* OverlappedCharacter;

};
