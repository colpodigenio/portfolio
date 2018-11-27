// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "portfolio_projectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Private/KismetTraceUtils.h"
#include "portfolio_project.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Weapon.h"

void Aportfolio_projectProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, HitPoint, FRotator::ZeroRotator, true);
	ProjectileShockWave->FireImpulse();
}

Aportfolio_projectProjectile::Aportfolio_projectProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &Aportfolio_projectProjectile::OnHit);		// set up a notification for when this component hits something blocking
	CollisionComp->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = false;
	

	// Radial Force defaults
	ProjectileShockWave = CreateDefaultSubobject<URadialForceComponent>(TEXT("ProjectileRadialForce"));
	ProjectileShockWave->SetupAttachment(RootComponent);
	ProjectileShockWave->Radius = 250.0f;
	ProjectileShockWave->bImpulseVelChange = true;
	ProjectileShockWave->bAutoActivate = false;
	ProjectileShockWave->bIgnoreOwningActor = true;

	// Fire trace effect defaults
	FireTraceEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireTraceEffect"));
	FireTraceEffect->SetupAttachment(RootComponent);
	FireTraceEffect->SetRelativeLocation(FVector(0.0f, -10.0f, 0.0f));

	// Die after 30 seconds by default
	InitialLifeSpan = 30.0f;

	// Default damage
	RocketDamage = 80.0f;
	RocketMinDamage = RocketDamage / 10.0f;

}

void Aportfolio_projectProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{

		HitPoint = Hit.ImpactPoint;

 		TArray<AActor*> IgnoreActors;
		AWeapon* MyOwner = Cast<AWeapon>(GetOwner());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Hit.ImpactPoint);
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), RocketDamage, RocketMinDamage, Hit.ImpactPoint, 70.0f, 300.0f, 5.0f, DamageType, IgnoreActors, nullptr,
			GetOwner()->GetInstigatorController(), ECC_Weapon);

		if (OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
		}

		Destroy();
	}
}
