// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "portfolio_projectProjectile.generated.h"

class URadialForceComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundBase;

UCLASS(config=Game)
class Aportfolio_projectProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Projectile, meta=(AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	// Projectile radial force component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class URadialForceComponent* ProjectileShockWave;

	// Particle effect on rocket trace
	UPROPERTY(VisibleAnywhere, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FireTraceEffect;

public:
	// Called if projectile is being removed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	Aportfolio_projectProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	// Returns radial force component
	FORCEINLINE class URadialForceComponent* GetProjectileRadialForce() const { return ProjectileShockWave; }
	// Returns fire trace effect
	FORCEINLINE class UParticleSystemComponent* GetFireTraceEffect() const { return FireTraceEffect; }

	// Rocket damage
	float RocketDamage;
	float RocketMinDamage;

protected:

	// Sound of explosion
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (BlueprintProtected = "true"))
	USoundBase* ExplosionSound;

	// Damage type class reference
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (BlueprintProtected = "true"))
	TSubclassOf<UDamageType> DamageType;

	// Particle effect on rocket explosion
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (BlueprintProtected = "true"))
	UParticleSystem* ExplosionEffect;

private:

	// Point where rocket hits target
	FVector HitPoint;

};

