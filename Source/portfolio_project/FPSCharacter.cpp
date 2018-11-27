// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "portfolio_projectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PawnMovementComponent.h"
#include "UnrealNetwork.h"
#include "Weapon.h"
#include "portfolio_project.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayGameMode.h"
#include "Explosives.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayPlayerController.h"
#include "DeathmatchGameState.h"
#include "GameplayPlayerState.h"
#include "DeathmatchGameMode.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// Aportfolio_projectCharacter

AFPSCharacter::AFPSCharacter()
{
	// Sets if actor can tick
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// Set capsule component to ignore Weapon channel
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);

	// Create camera spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	FName SocketName = TEXT("GripPoint");
	SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArm);//GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Explosive spawning point
	ExplosivesSpawningPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ExplosiveSpawningPoint"));
	ExplosivesSpawningPoint->SetupAttachment(FirstPersonCameraComponent);
	ExplosivesSpawningPoint->SetRelativeLocation(FVector(30.0f, 60.0f, 0.0f));
	ExplosivesSpawningPoint->SetRelativeRotation(FRotator(0.0, 0.0f, 0.0f));

	//Replicates this
	SetReplicates(true);
	SetReplicateMovement(true);

	// Enables crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Default zooming state (false)
	bIsZooming = false;

	// Default zoom value
	ZoomedSight = 60.0f;
	// Default sight interpolation speed
	SightInterpSpeed = 15.0f;

	// Create Health component and bind it to delegate
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AFPSCharacter::ReceiveHealthChanged);

	// Default bIsDead
	bIsDead = false;

	// AMMUNITION DEFAULTS

	AssaultRifleAmmoAmount = 30;
	MaxAssaultRifleAmmoAmount = 180;
	AssaultRifleMagazineCapacity = 30;
	AssaultRifleAmmoInMagazine = 30;

	ShotgunAmmoAmount = 0;
	MaxShotgunAmmoAmount = 40;
	ShotgunMagazineCapacity = 8;
	ShotgunAmmoInMagazine = 0;

	RocketLauncherAmmoAmount = 0;
	MaxRocketLauncherAmmoAmount = 10;	
	RocketLauncherMagazineCapacity = 1;
	RocketLauncherAmmoInMagazine = 0;

	GrenadesAmount = 0;
	MaxGrenadesAmount = 8;

	MinesAmount = 0;
	MaxMinesAmount = 6;

	ExplosivesUsingDelay = 1.0f;
	bIsExplosiveUsed = false;
	
	AssaultRifle = nullptr;
	Shotgun = nullptr;
	RocketLauncher = nullptr;

	bIsPowerupActive = false;
	bIsDamageBoosted = false;
	bIsSpeedBoosted = false;
	bIsHealthRegenerating = false;

	bIsAssaultRifleBoosted = false;
	bIsShotgunBoosted = false;
	bIsRocketLauncherBoosted = false;

	bIsThrowGrenadeHeld = false;
	ThrowGrenadeHoldTime = 0.0f;
	GrenadeTimeBeforeExplode = 3.0f;

	Grenade = nullptr;

	FallingDamage = 0.0f;
	bWasFalling = false;
	bHUDCanBeDestroyed = false;
}

void AFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	// Sets default camera field of view
	NormalSight = FirstPersonCameraComponent->FieldOfView;

	FActorSpawnParameters CurrentWeaponSpawnParameters;
	CurrentWeaponSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeaponSpawnParameters.Owner = this;
	if (Role == ROLE_Authority)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, CurrentWeaponSpawnParameters);
		FName WeaponSocketName = TEXT("GripPoint");
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		CurrentWeaponType = EWeaponType::AssaultRifle;
		AssaultRifle = CurrentWeapon;
	}
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Zoom();
	TakeFallDamage();
	SetAimPitch();
	BoostDamage();
	if (HasAuthority())
	{
		SetGrenadeThrowingPower();
	}
	CorrectAccuracy();
	if (!PlayerController && GetController())
	{
		PlayerController = Cast<AGameplayPlayerController>(GetController());
	}
}

void AFPSCharacter::SetAimPitch()
{
	if (!IsLocallyControlled())
	{
		AimPitch = FMath::ClampAngle((RemoteViewPitch * 360.0f / 255.0f), -90.0f, 90.0f);
	}
	else
	{
		AimPitch = FMath::ClampAngle(GetControlRotation().Pitch, -90.0f, 90.0f);
	}
}



void AFPSCharacter::CorrectAccuracy()
{
	// Accuracy correction on movement and zoom
	if (CurrentWeapon)
	{
		if (FVector(GetMovementComponent()->Velocity).Size() > 0.0f && bIsZooming)
		{
			FinalAimCorrection = CurrentWeapon->TotalAimCorrection;
		}
		else if (FVector(GetMovementComponent()->Velocity).Size() > 0.0f && !bIsZooming)
		{
			FinalAimCorrection = CurrentWeapon->MovementAimCorrection;
		}
		else if (FVector(GetMovementComponent()->Velocity).Size() <= 0.0f && bIsZooming)
		{
			FinalAimCorrection = CurrentWeapon->ZoomAimCorrection;
		}
		else
		{
			FinalAimCorrection = CurrentWeapon->DefaultAimCorrection;
		}
	}
}

void AFPSCharacter::SetGrenadeThrowingPower()
{
	// Grenade throw power
	if (bIsThrowGrenadeHeld)
	{
		ThrowGrenadeHoldTime += GetWorld()->GetDeltaSeconds();
		if (ThrowGrenadeHoldTime >= GrenadeTimeBeforeExplode)
		{
			ServerEndThrowGrenade();
			ThrowGrenade(0.0f);
			Grenade->TimeBeforeGrenadeExplodes = 0.01f;
			Grenade->ExplosiveTimer();
			ThrowGrenadeHoldTime = 0.0f;
		}
	}
	else if (!bIsThrowGrenadeHeld && ThrowGrenadeHoldTime > 0.0f)
	{
		ThrowGrenade(ThrowGrenadeHoldTime / GrenadeTimeBeforeExplode);
		Grenade->TimeBeforeGrenadeExplodes -= ThrowGrenadeHoldTime;
		Grenade->ExplosiveTimer();
		ThrowGrenadeHoldTime = 0.0f;
	}
}

void AFPSCharacter::BoostDamage()
{
	// Boost weapons damage
	if (!bIsDamageBoosted)
	{
		bIsAssaultRifleBoosted = false;
		bIsShotgunBoosted = false;
		bIsRocketLauncherBoosted = false;
	}
	if (bIsDamageBoosted)
	{
		if (AssaultRifle && !bIsAssaultRifleBoosted)
		{
			bIsAssaultRifleBoosted = true;
			AssaultRifle->DamageBoost();
		}
		if (Shotgun && !bIsShotgunBoosted)
		{
			bIsShotgunBoosted = true;
			Shotgun->DamageBoost();
		}
		if (RocketLauncher && !bIsRocketLauncherBoosted)
		{
			bIsRocketLauncherBoosted = true;
			RocketLauncher->DamageBoost();
		}
	}
}

void AFPSCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	UGameplayStatics::ApplyDamage(this, 10000.0f, GetController(), this, nullptr);
}

void AFPSCharacter::TakeFallDamage()
{
	// Damage on fall
	if (GetVelocity().Z <= -1200.0f)
	{
		FallingDamage = GetVelocity().Z / -30.0f;
		bWasFalling = true;
	}
	else if (!GetCharacterMovement()->IsFalling() && bWasFalling)
	{
		bWasFalling = false;
		HealthComponent->Health -= FallingDamage;
		if (HealthComponent->Health <= 0.0f)
		{
			Dying();
		}
	}
}

void AFPSCharacter::Zoom()
{
	// Zoom implementation
	float ValidSight = bIsZooming ? ZoomedSight : NormalSight;

	float NewSight = FMath::FInterpTo(FirstPersonCameraComponent->FieldOfView, ValidSight, GetWorld()->GetDeltaSeconds(), SightInterpSpeed);

	FirstPersonCameraComponent->SetFieldOfView(NewSight);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::StopFire);

	// Bind crouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSCharacter::EndCrouch);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// Bind turn events
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Bind zoom event
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFPSCharacter::EndZoom);

	// Bind weapon reload
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::ServerBeginReload);

	// Bind change weapon
	PlayerInputComponent->BindAction("AssaultRifle", IE_Pressed, this, &AFPSCharacter::ChangeWeapon1);
	PlayerInputComponent->BindAction("Shotgun", IE_Pressed, this, &AFPSCharacter::ChangeWeapon2);
	PlayerInputComponent->BindAction("RocketLauncher", IE_Pressed, this, &AFPSCharacter::ChangeWeapon3);

	// Bind grenade use
	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &AFPSCharacter::ServerBeginThrowGrenade);
	PlayerInputComponent->BindAction("Grenade", IE_Released, this, &AFPSCharacter::ServerEndThrowGrenade);

	// Bind remote bomb use
	PlayerInputComponent->BindAction("RemoteBomb", IE_Pressed, this, &AFPSCharacter::ServerSetMine);

}

void AFPSCharacter::BeginCrouch()
{
	Crouch();
}

void AFPSCharacter::EndCrouch()
{
	UnCrouch();
}

void AFPSCharacter::BeginZoom()
{
	bIsZooming = true;
}

void AFPSCharacter::EndZoom()
{
	bIsZooming = false;
}

void AFPSCharacter::ServerBeginReload_Implementation()
{
	switch (CurrentWeaponType)
	{
	case EWeaponType::AssaultRifle:
		if (AssaultRifleAmmoAmount > 0 && AssaultRifleAmmoInMagazine < AssaultRifleMagazineCapacity)
		{
			CurrentWeapon->Reload();
			bIsReloading = true;
		}
		break;
	case EWeaponType::Shotgun:
		if (ShotgunAmmoAmount > 0 && ShotgunAmmoInMagazine < ShotgunMagazineCapacity)
		{	
			CurrentWeapon->Reload();
			bIsReloading = true;		
		}
		break;
	case EWeaponType::RocketLauncher:
		if (RocketLauncherAmmoAmount > 0 && RocketLauncherAmmoInMagazine < RocketLauncherMagazineCapacity)
		{			
			CurrentWeapon->Reload();
			bIsReloading = true;			
		}
		break;
	}
}

bool AFPSCharacter::ServerBeginReload_Validate()
{
	return true;
}


void AFPSCharacter::ServerBeginThrowGrenade_Implementation()
{
	ADeathmatchGameState* DeathmatchGameStateRef = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (GrenadesAmount > 0 && !bIsExplosiveUsed && DeathmatchGameStateRef->bMatchHasStarted)
	{
		bIsExplosiveUsed = true;
		GrenadesAmount--;
		bIsThrowGrenadeHeld = true;
		GetWorldTimerManager().SetTimer(ExplosiveUseDelay_Timer, this, &AFPSCharacter::SetIsExplosiveUsedFalse, ExplosivesUsingDelay, false);
	}
}

bool AFPSCharacter::ServerBeginThrowGrenade_Validate()
{
	return true;
}

void AFPSCharacter::ThrowGrenade(float ThrowPower)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = this;
	Grenade = GetWorld()->SpawnActor<AExplosives>(GrenadeClass, ExplosivesSpawningPoint->GetComponentLocation(), ExplosivesSpawningPoint->GetComponentRotation(), SpawnParameters);
	Grenade->GetProjectileMovement()->Velocity += ThrowPower * Grenade->GetProjectileMovement()->Velocity;
	Grenade->GetProjectileMovement()->Velocity += GetVelocity();
	Grenade->TimeBeforeGrenadeExplodes = GrenadeTimeBeforeExplode;
}

void AFPSCharacter::ServerEndThrowGrenade_Implementation()
{
	bIsThrowGrenadeHeld = false;
}

bool AFPSCharacter::ServerEndThrowGrenade_Validate()
{
	return true;
}

void AFPSCharacter::ServerSetMine_Implementation()
{
	ADeathmatchGameState* DeathmatchGameStateRef = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (MinesAmount > 0 && !bIsExplosiveUsed && DeathmatchGameStateRef->bMatchHasStarted)
	{
		bIsExplosiveUsed = true;
		MinesAmount--;
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Owner = this;
		AExplosives* Mine = GetWorld()->SpawnActor<AExplosives>(RemoteBombClass, ExplosivesSpawningPoint->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f), SpawnParameters);
		Mine->GetProjectileMovement()->SetVelocityInLocalSpace(FVector::ZeroVector);
		Mine->GetSphereComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetWorldTimerManager().SetTimer(ExplosiveUseDelay_Timer, this, &AFPSCharacter::SetIsExplosiveUsedFalse, ExplosivesUsingDelay, false);
	}
}

bool AFPSCharacter::ServerSetMine_Validate()
{
	return true;
}

void AFPSCharacter::SetIsExplosiveUsedFalse()
{
	bIsExplosiveUsed = false;
}

void AFPSCharacter::ReceiveHealthChanged(UHealthComponent* HealthComp, float Health, float Armor, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDead)
	{	
		AGameplayPlayerController* Murderer = Cast<AGameplayPlayerController>(InstigatedBy);
		ADeathmatchGameMode* DeathmatchGameModeRef = Cast<ADeathmatchGameMode>(GetWorld()->GetAuthGameMode());
		if (Murderer && DeathmatchGameModeRef->bMatchHasStarted)
		{
			AGameplayPlayerState* MurdererPlayerState = Cast<AGameplayPlayerState>(Murderer->PlayerState);
			Murderer != this->GetController() ?	MurdererPlayerState->EnemiesKilled++ : MurdererPlayerState->EnemiesKilled--;
		}
		Dying();
	}
}

void AFPSCharacter::OnRep_IsDead()
{
	Dying();
}

void AFPSCharacter::Dying()
{
	bIsDead = true;
	bHUDCanBeDestroyed = true;
	GetMovementComponent()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	DetachFromControllerPendingDestroy();

	FTimerHandle Respawn_Timer;
	GetWorldTimerManager().SetTimer(Respawn_Timer, this, &AFPSCharacter::Respawn, 5.0f, false);	
}

void AFPSCharacter::Respawn()
{
	Cast<ADeathmatchGameState>(GetWorld()->GetGameState())->ServerRespawn(PlayerController);
	CurrentWeapon->SetLifeSpan(1.0f);
	SetLifeSpan(1.0f);
}

void AFPSCharacter::StartFire()
{
	ADeathmatchGameState* DeathmatchGameStateRef = Cast<ADeathmatchGameState>(GetWorld()->GetGameState());
	if (CurrentWeapon && DeathmatchGameStateRef->bMatchHasStarted)
	{
		CurrentWeapon->StartFire();
	}
}

void AFPSCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(AFPSCharacter, AssaultRifle);
	DOREPLIFETIME(AFPSCharacter, Shotgun);
	DOREPLIFETIME(AFPSCharacter, RocketLauncher);
	DOREPLIFETIME(AFPSCharacter, bIsDead);
	DOREPLIFETIME(AFPSCharacter, bHUDCanBeDestroyed);
	DOREPLIFETIME_CONDITION(AFPSCharacter, AssaultRifleAmmoAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, AssaultRifleAmmoInMagazine, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, ShotgunAmmoAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, ShotgunAmmoInMagazine, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, RocketLauncherAmmoAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, RocketLauncherAmmoInMagazine, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, GrenadesAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, Grenade, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, bIsThrowGrenadeHeld, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, ThrowGrenadeHoldTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, GrenadeTimeBeforeExplode, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, MinesAmount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, CurrentWeaponType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, bIsPowerupActive, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, bIsReloading, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, bIsExplosiveUsed, COND_OwnerOnly);
}

