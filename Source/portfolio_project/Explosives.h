// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosives.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class URadialForceComponent;
class UProjectileMovementComponent;
class USphereComponent;
class USoundBase;

UCLASS()
class PORTFOLIO_PROJECT_API AExplosives : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosives();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;

	// If it explodes on overlap or on timer
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected = "true"))
	bool bExplodesAfterOverlap;

	// Damage from the explosion
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected = "true"))
	float ExplosiveDamage;

	// Sound of explosion
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (BlueprintProtected = "true"))
	USoundBase* ExplosionSound;

	// Explosion effect
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected))
	UParticleSystem* ExplosionEffect;

	// Damage type
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected))
	TSubclassOf<UDamageType> DamageTypeClass;

public:
	// Time before explosion
	float TimeBeforeGrenadeExplodes;

	UPROPERTY(EditDefaultsOnly, Category = "Explosives")
	float TimeBeforeMineExplodes;

	// Triggers explosive's timer, after it runs out it calls Explode()
	void ExplosiveTimer();

private:
	// Creates static mesh
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	// Creates sphere component for overlap events
	UPROPERTY(VisibleDefaultsOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	// Creates radial force component
	UPROPERTY(VisibleDefaultsOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* ExplosionShockWave;

	// Creates projectile movement component
	UPROPERTY(VisibleDefaultsOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Explode this
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastExplode();

	// Explosive timer handle
	FTimerHandle Explosive_Timer;

	// On take any damage
	UFUNCTION()
	void NotifyOnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
public:
	// Returns mesh component
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	// Returns sphere component
	FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }
	// Returns radial force component
	FORCEINLINE URadialForceComponent* GetExplosiveShockWave() const { return ExplosionShockWave; }
	// Returns projectile movement component
	UFUNCTION(BlueprintPure)
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
