// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "portfolio_project.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"


// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
	// Creates mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionResponseToChannel(ECC_Weapon, ECR_Block);
	Mesh->SetUseCCD(true);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetGenerateOverlapEvents(false);
	RootComponent = Mesh;

	// Creates radial force component
	ExplosionShockWave = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionShockWave"));
	ExplosionShockWave->SetupAttachment(RootComponent);
	ExplosionShockWave->Radius = 250.0f;
	ExplosionShockWave->bImpulseVelChange = true;
	ExplosionShockWave->bAutoActivate = false;
	ExplosionShockWave->bIgnoreOwningActor = true;
	ExplosionShockWave->ImpulseStrength = 500.0f;

	// Creates health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->Health = 30.0f;
	HealthComponent->Armor = 0.0f;
	HealthComponent->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::ReceiveHealthChanged);

	FireParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire"));
	FireParticleEffect->SetupAttachment(RootComponent);
	FireParticleEffect->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	FireParticleEffect->bAutoActivate = false;	

	// Defaults
	ExplosiveDamage = 50.0f;
	TimeBeforeBarrelExplodes = 0.7f;
	bIsExploding = false;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AExplosiveBarrel::MulticastExplode_Implementation()
{
	Mesh->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);
	float ExplosiveMinDamage = ExplosiveDamage / 10.0f;
	TArray<AActor*> IgnoreActors;
	ExplosionShockWave->FireImpulse();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ExplosiveDamage, ExplosiveMinDamage, GetActorLocation(), 150.0f, 300.0f, 5.0f, DamageTypeClass, IgnoreActors, this,
		nullptr, ECC_Weapon);
	FireParticleEffect->DeactivateSystem();
	Destroy();
}

bool AExplosiveBarrel::MulticastExplode_Validate()
{
	return true;
}

void AExplosiveBarrel::MulticastActivateFireEffect_Implementation()
{
	FireParticleEffect->ActivateSystem(false);
}

bool AExplosiveBarrel::MulticastActivateFireEffect_Validate()
{
	return true;
}

void AExplosiveBarrel::ReceiveHealthChanged(UHealthComponent* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= -20.0f)
	{
		GetWorldTimerManager().ClearTimer(Explosive_Timer);
		MulticastExplode();
	}
	if (Health <= 0.0f && !bIsExploding)
	{
		bIsExploding = true;
		MulticastActivateFireEffect();
		GetWorldTimerManager().SetTimer(Explosive_Timer, this, &AExplosiveBarrel::MulticastExplode, TimeBeforeBarrelExplodes, false);
		return;
	}
	if (bIsExploding && GetWorldTimerManager().GetTimerRemaining(Explosive_Timer) > 0.0f)
	{
		GetWorldTimerManager().ClearTimer(Explosive_Timer);
		MulticastExplode();
	}
}
