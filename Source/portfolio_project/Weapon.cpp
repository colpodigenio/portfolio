// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Private/KismetTraceUtils.h"
#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "portfolio_project.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystem.h"
#include "Pickup.h"
#include "UnrealNetwork.h"
#include "portfolio_projectProjectile.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Creates console variable which shows/hides debug lines from weapon while firing
static int32 DebugWeaponDraw = 0;
FAutoConsoleVariableRef CVARDebugWeaponDraw(
	TEXT("Draw.Debug"),
	DebugWeaponDraw,
	TEXT("Draws line frome weapon muzzle to show where it fires."),
	ECVF_Cheat);

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Creates weapon mesh and assign it as root component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	// Creates Muzzle offset;
	MuzzleOffset = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleOffset"));
	MuzzleOffset->SetupAttachment(WeaponMesh);
	// Sets Muzzle socket name
	MuzzleSocketName = "MuzzleSocket";

	// Default damage
	DefaultWeaponDamage = 7.0f;

	// Default rate of fire
	FireRate = 100;

	// Default first shot fired
	bFirstShotFired = false;

	// Replicates this
	SetReplicates(true);
	
	// Defines how often object will be replicated
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	// Default bullet spread
	DefaultBulletSpread = 0.5f;

	// Default reloading values
	ReloadTime = 5.0f;

	AmountOfBulletsOnOneShot = 1;
	BulletsCounter = 0;

	// Default aim corrections

	ZoomAimCorrection = 0.2f;
	MovementAimCorrection = 2.0f;
	TotalAimCorrection = 1.4f;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60 / FireRate;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AWeapon::ServerFire_Validate()
{
	return true;
}

void AWeapon::StartFire()
{
	// Prevents player from firing faster than FireRate by clicking rapidly on fire button
	float TimerDelay = bFirstShotFired ? FMath::Max(TimeBetweenShots - (GetWorld()->TimeSeconds - LastShotTime), 0.0f) : 0.0f;

	bFirstShotFired = true;

	GetWorldTimerManager().SetTimer(Fire_Timer, this, &AWeapon::Fire, TimeBetweenShots, true, TimerDelay);
}

void AWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(Fire_Timer);
}

//fires weapon
void AWeapon::Fire()
{	
	// Runs function on server if it is called by client
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
	
	// Sets WeaponDamage
	WeaponDamage = DefaultWeaponDamage;

	AFPSCharacter* MyOwner = Cast<AFPSCharacter>(GetOwner());
	if (MyOwner)
	{
		// Decrease ammo number
		if (MyOwner->bIsReloading == false)
		{
			switch (MyOwner->CurrentWeaponType)
			{
			case EWeaponType::AssaultRifle:
				if (MyOwner->AssaultRifleAmmoInMagazine > 0 && !MyOwner->bIsDead)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, MuzzleOffset->GetComponentLocation());
					OneBulletShot();
					MyOwner->AssaultRifleAmmoInMagazine--;
				}
				else
				{
					// Stop firing if there is no ammo in magazine
					return;
				}
				break;
			case EWeaponType::Shotgun:
				if (MyOwner->ShotgunAmmoInMagazine > 0 && !MyOwner->bIsDead)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, MuzzleOffset->GetComponentLocation());
					OneBulletShot();
					MyOwner->ShotgunAmmoInMagazine--;
				}
				else
				{
					// Stop firing if there is no ammo in magazine
					return;
				}
				break;
			case EWeaponType::RocketLauncher:
				if (MyOwner->RocketLauncherAmmoInMagazine > 0 && !MyOwner->bIsDead)
				{
					if (HasAuthority())
					{
						MulticastOneRocketShot();
					}
					if (MyOwner->RocketLauncherAmmoAmount > 0)
					{
						Reload();
					}
					MyOwner->RocketLauncherAmmoInMagazine--;
				}
				else
				{
					// Stop firing if there is no ammo in magazine
					return;
				}
				break;
			}
		}
		else
		{
			// Do not fire if character is reloading
			return;
		}

		
	}

	
}

void AWeapon::OneBulletShot()
{

	BulletsCounter++;

	
	AFPSCharacter* MyOwner = Cast<AFPSCharacter>(GetOwner());
	// Query parameters for line trace
	FCollisionQueryParams  QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	// Defining of line trace start and end location
	FVector ShotDirection = MyOwner->GetViewRotation().Vector();
	// Correction on bullet spread
	BulletSpread = MyOwner->FinalAimCorrection * DefaultBulletSpread;
	float HalfRadian = FMath::DegreesToRadians(BulletSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRadian);

	FVector LineStart = MyOwner->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector LineEnd = LineStart + ShotDirection * 10000;

	FHitResult ResultOfHit;

	// Default Surface type
	EPhysicalSurface TargetsSurfaceType = SurfaceType_Default;

	if (GetWorld()->LineTraceSingleByChannel(ResultOfHit, LineStart, LineEnd, ECC_Weapon, QueryParams))
	{
		// Draw debug line to show trace
		if (DebugWeaponDraw > 0)
		{
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		}
		// Draw debug sphere to show impact point
		if (DebugWeaponDraw > 0)
		{
			if (Cast<AFPSCharacter>(ResultOfHit.GetActor()))
			{
				DrawDebugSphere(GetWorld(), ResultOfHit.ImpactPoint, 5.0f, 8, FColor::Black, false, 2.0f, 0, 1.0f);
			}
		}

		// Chooses which impact effect to play//ResultOfHit.PhysMaterial.Get() .Get() is a weak ptr which will be destroyed after use
		TargetsSurfaceType = UPhysicalMaterial::DetermineSurfaceType(ResultOfHit.PhysMaterial.Get());

		// Plays impact effect
		PlayImpactEffect(TargetsSurfaceType, ResultOfHit.ImpactPoint);

		UGameplayStatics::ApplyPointDamage(ResultOfHit.GetActor(), WeaponDamage, ShotDirection, ResultOfHit, MyOwner->GetInstigatorController(), GetOwner(), DamageType);
		
	}


	// Shakes camera while firing

	APlayerController* PlayerControllerRef = Cast<APlayerController>(MyOwner->GetController());
	if (PlayerControllerRef)
	{
		PlayerControllerRef->ClientPlayCameraShake(WeaponCameraShake);
	}
	// Makes fire effect at weapon muzzle while it is firing
	if (MuzzleFireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFireEffect, WeaponMesh, MuzzleSocketName);
	}

	// Changes HitScanTrace value and calls OnRep_HitScanTrace on that change. Calls it when surface type changes or impact location changes
	if (Role == ROLE_Authority)
	{
		HitScanTrace.TraceTo = ResultOfHit.ImpactPoint;
		HitScanTrace.SurfaceType = TargetsSurfaceType;
	}

	// Sets Last shot time
	LastShotTime = GetWorld()->TimeSeconds;

	if (BulletsCounter < AmountOfBulletsOnOneShot)
	{
		OneBulletShot();
	}
	BulletsCounter = 0;
}

void AWeapon::MulticastOneRocketShot_Implementation()
{	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this->GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Aportfolio_projectProjectile* Rocket = GetWorld()->SpawnActor<Aportfolio_projectProjectile>(RocketClass, MuzzleOffset->GetComponentLocation(),
		MuzzleOffset->GetComponentRotation(), SpawnParameters);
	if (Rocket)
	{
		Rocket->RocketDamage = WeaponDamage;
		Rocket->RocketMinDamage = WeaponDamage / 10.0f;
		// Shot direction
		FRotator OutWeaponWorldRotation;
		FVector OutNewLocation;
		if (GetOwner())
		{
			GetOwner()->GetActorEyesViewPoint(OutNewLocation, OutWeaponWorldRotation);
			FVector RocketFlightDirection = OutWeaponWorldRotation.Vector() * Rocket->GetProjectileMovement()->InitialSpeed;
			Rocket->GetProjectileMovement()->SetVelocityInLocalSpace(RocketFlightDirection);
		}
	}
}

bool AWeapon::MulticastOneRocketShot_Validate()
{
	return true;
}

void AWeapon::OnRep_HitScanTrace()
{
	
	// Makes fire effect at weapon muzzle while it is firing
	if (MuzzleFireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFireEffect, WeaponMesh, MuzzleSocketName);
	}
	// Plays impact effect
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void AWeapon::ReloadDelay()
{
	AFPSCharacter* MyOwner = Cast<AFPSCharacter>(GetOwner());
	if (MyOwner)
	{
		MyOwner->bIsReloading = false;
		switch (MyOwner->CurrentWeaponType)
		{
		case EWeaponType::AssaultRifle:
			MyOwner->AssaultRifleAmmoAmount -= MyOwner->AssaultRifleMagazineCapacity;
			MyOwner->AssaultRifleAmmoInMagazine = MyOwner->AssaultRifleMagazineCapacity;
			break;

		case EWeaponType::Shotgun:
			MyOwner->ShotgunAmmoAmount -= MyOwner->ShotgunMagazineCapacity;
			MyOwner->ShotgunAmmoInMagazine = MyOwner->ShotgunMagazineCapacity;
			break;

		case EWeaponType::RocketLauncher:
			MyOwner->RocketLauncherAmmoAmount -= MyOwner->RocketLauncherMagazineCapacity;
			MyOwner->RocketLauncherAmmoInMagazine = MyOwner->RocketLauncherMagazineCapacity;
			break;					
		}
	}	
}

void AWeapon::DamageBoost()
{
	UE_LOG(LogTemp, Warning, TEXT("Damage boost started"))
	DefaultWeaponDamage *= 3.0f;
}

void AWeapon::DamageBoostEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Damage boost ended"))
	DefaultWeaponDamage /= 3.0f;
}

void AWeapon::Reload()
{
	AFPSCharacter* MyOwner = Cast<AFPSCharacter>(GetOwner());
	if (MyOwner && !(MyOwner->bIsReloading))
	{
		GetWorldTimerManager().SetTimer(Reload_Timer, this, &AWeapon::ReloadDelay, ReloadTime, false);
	}
}

void AWeapon::PlayImpactEffect(EPhysicalSurface NewTargetsSurfaceType, FVector NewImpactPoint)
{
	// Local variable which stores selected surface type and passes it to spawn function 
	UParticleSystem* SelectedImpactEffect = nullptr;

	// if shot hits head then damage dealt increases 
	if (NewTargetsSurfaceType == Surface_FleshHead)
	{
		WeaponDamage = 3.0f * DefaultWeaponDamage;
	}

	// Sets which impact effect to use depending on surface type
	switch (NewTargetsSurfaceType)
	{
	case Surface_FleshBody:
	case Surface_FleshHead:
		SelectedImpactEffect = FleshImpactEffect;
		break;
	default:
		SelectedImpactEffect = DefaultImpactEffect;
		break;
	}

	// Makes impact particle effect on point of impact
	if (SelectedImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, NewImpactPoint);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon, HitScanTrace, COND_SkipOwner);
	
}

