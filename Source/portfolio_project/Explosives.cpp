// Fill out your copyright notice in the Description page of Project Settings.

#include "Explosives.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "portfolio_project.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"


// Sets default values
AExplosives::AExplosives()
{
	// Creates mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionResponseToChannel(ECC_Weapon, ECR_Block);
	Mesh->SetUseCCD(true);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetGenerateOverlapEvents(false);
	RootComponent = Mesh;

	// Creates sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(200.0f);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetCanEverAffectNavigation(false);


	// Creates radial force component
	ExplosionShockWave = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionShockWave"));
	ExplosionShockWave->SetupAttachment(RootComponent);
	ExplosionShockWave->Radius = 250.0f;
	ExplosionShockWave->bImpulseVelChange = true;
	ExplosionShockWave->bAutoActivate = false;
	ExplosionShockWave->bIgnoreOwningActor = true;
	ExplosionShockWave->ImpulseStrength = 500.0f;

	// Creates projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.2f;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->SetVelocityInLocalSpace(FVector(1.0f, -0.05f, 0.0f));

	// Defaults
	bExplodesAfterOverlap = false;
	ExplosiveDamage = 70.0f;
	TimeBeforeMineExplodes = 0.4f;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AExplosives::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &AExplosives::NotifyOnTakeAnyDamage);
}


void AExplosives::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bExplodesAfterOverlap && OtherActor != GetOwner() && StaticClass() != OtherActor->GetClass()) // doesn't trigger overlap event if overlapped actor is character that spawns this
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *OtherActor->GetName())
		UE_LOG(LogTemp, Log, TEXT("%s"), *GetOwner()->GetName())
		GetWorldTimerManager().SetTimer(Explosive_Timer, this, &AExplosives::MulticastExplode, TimeBeforeMineExplodes, false);
	}
}

void AExplosives::MulticastExplode_Implementation()
{
	Mesh->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);
	OnTakeAnyDamage.RemoveDynamic(this, &AExplosives::NotifyOnTakeAnyDamage);
	float ExplosiveMinDamage = ExplosiveDamage / 10.0f;
	TArray<AActor*> IgnoreActors;
	ExplosionShockWave->FireImpulse();
 	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
 	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ExplosiveDamage, ExplosiveMinDamage, GetActorLocation(), 100.0f, 250.0f, 5.0f, DamageTypeClass, IgnoreActors, nullptr,
 		GetOwner()->GetInstigatorController(), ECC_Weapon);
	Destroy();
}

bool AExplosives::MulticastExplode_Validate()
{
	return true;
}

void AExplosives::ExplosiveTimer()
{
	
	GetWorldTimerManager().SetTimer(Explosive_Timer, this, &AExplosives::MulticastExplode, TimeBeforeGrenadeExplodes, false);
	
}

void AExplosives::NotifyOnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	GetWorldTimerManager().ClearTimer(Explosive_Timer);
	MulticastExplode();
}

