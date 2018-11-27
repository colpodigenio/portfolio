// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class UParticleSystemComponent;
class URadialForceComponent;
class UHealthComponent;
class USoundBase;

UCLASS()
class PORTFOLIO_PROJECT_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:

	// Damage from the explosion
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected = "true"))
	float ExplosiveDamage;

	// Sound of explosion
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (BlueprintProtected = "true"))
	USoundBase* ExplosionSound;

	// Explosion effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosives", meta = (BlueprintProtected = "true"))
	UParticleSystem* ExplosionEffect;

	// Damage type
	UPROPERTY(EditDefaultsOnly, Category = "Explosives", meta = (BlueprintProtected = "true"))
	TSubclassOf<UDamageType> DamageTypeClass;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Explosives")
	float TimeBeforeBarrelExplodes;

private:

	// Creates static mesh
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FireParticleEffect;

	// Creates static mesh
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	// Creates radial force component
	UPROPERTY(VisibleDefaultsOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* ExplosionShockWave;

	// Creates health component that will handle taken damage
	UPROPERTY(VisibleDefaultsOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	// Defines if barrel would explode
	bool bIsExploding;

	// Multicasrt explode this
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastExplode();

	// Multicast activate fire particle effect
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastActivateFireEffect();

	// Explosive timer handle
	FTimerHandle Explosive_Timer;

	UFUNCTION()
	void ReceiveHealthChanged(UHealthComponent* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

public:
	// Returns mesh component
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	// Returns sphere component
	FORCEINLINE URadialForceComponent* GetExplosiveShockWave() const { return ExplosionShockWave; }
	// Returns health component
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	
};
