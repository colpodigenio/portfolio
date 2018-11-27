// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pickup.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class AWeapon;
class UHealthComponent;
class AExplosives;
class USceneComponent;
class USpringArmComponent;
class AGameplayPlayerController;

UCLASS(config = Game)
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	// Creates Health component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	// Explosives spawning point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ExplosivesSpawningPoint;

	// Camera spring arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;


public:
	AFPSCharacter();

protected:
	virtual void BeginPlay();

public:
	virtual void Tick(float DeltaTime) override;

	// Kills character when he reaches certain Z level
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

protected:

	// Player controller reference
	AGameplayPlayerController* PlayerController;

	//Defines if projectiles are being fired
	bool bIsFiring = false;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	// Returns camera
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	// Return explosives spawning point
	FORCEINLINE USceneComponent* GetExplosivesSpawningPoint() const { return ExplosivesSpawningPoint; }
	// Returns camera spring arm
	// Return explosives spawning point
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/**CROUCH*/
protected:
	UFUNCTION()
	void BeginCrouch();

	UFUNCTION()
	void EndCrouch();

	/**WEAPON LOGICS*/
protected:

	// Begin to zoom
	void BeginZoom();

	// End to zoom
	void EndZoom();

	// True if character is zooming
	bool bIsZooming;

	// Default camera's field of view
	float NormalSight;

	// Field of view while zooming in
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected ="true"))
	float ZoomedSight;

	// Speed of interpolation between sights
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 1.0, ClampMax = 50.0f, BlueprintProtected = "true"))
	float SightInterpSpeed;




public:

	// Server weapon reload implementation
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Weapon")
	void ServerBeginReload();

	// True if reloading timer has been started and false if it has been ended
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading;
	
	// Sets weapon type that character is using right now
 	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
 	EWeaponType CurrentWeaponType;

	// Sets powerupType which is active right now, can be activated only one at a time
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EPowerupType CurrentPowerupType;

	// Zoom correction of aim, when zoom aim is better (Lower the value - higher the accuracy)
	float FinalAimCorrection;

	// Starts firing a projectile
	UFUNCTION(BlueprintCallable, Category = "Character")
	void StartFire();

	// Stops firing a projectile
	UFUNCTION(BlueprintCallable, Category = "Character")
	void StopFire();

	// Takes 1 weapon used by character
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void ChangeWeapon1();

	// Takes 2 weapon used by character
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void ChangeWeapon2();

	// Takes 3 weapon used by character
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void ChangeWeapon3();

	/**AMMUNITION*/

	// Weapons
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
	AWeapon* AssaultRifle;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
	AWeapon* Shotgun;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
	AWeapon* RocketLauncher;

	// Holds reference of weapon that is being used right now
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeapon* CurrentWeapon;

	// Assault Rifle ammunition
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "AssaultRifle")
	int32 AssaultRifleAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssaultRifle")
	int32 MaxAssaultRifleAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssaultRifle")
	int32 AssaultRifleMagazineCapacity;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "AssaultRifle")
	int32 AssaultRifleAmmoInMagazine;

	// Shotgun ammunition
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Shotgun")
	int32 ShotgunAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shotgun")
	int32 MaxShotgunAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shotgun")
	int32 ShotgunMagazineCapacity;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Shotgun")
	int32 ShotgunAmmoInMagazine;

	// Rocket Launcher ammunition
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "RocketLauncher")
	int32 RocketLauncherAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RocketLauncher")
	int32 MaxRocketLauncherAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RocketLauncher")
	int32 RocketLauncherMagazineCapacity;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "RocketLauncher")
	int32 RocketLauncherAmmoInMagazine;

	// Grenades
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Explosives")
	int32 GrenadesAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosives")
	int32 MaxGrenadesAmount;

	// Remote bombs
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Explosives")
	int32 MinesAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosives")
	int32 MaxMinesAmount;

	// Is any powerup active
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Powerups")
	bool bIsPowerupActive;

	UPROPERTY(BlueprintReadWrite, Category = "Powerups")
	bool bIsDamageBoosted;
	UPROPERTY(BlueprintReadWrite, Category = "Powerups")
	bool bIsSpeedBoosted;
	UPROPERTY(BlueprintReadWrite, Category = "Powerups")
	bool bIsHealthRegenerating;

	bool bIsAssaultRifleBoosted;
	bool bIsShotgunBoosted;
	bool bIsRocketLauncherBoosted;

	// Grenade instance
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Explosives")
	AExplosives* Grenade;

	// If character throws grenade
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Explosives")
	bool bIsThrowGrenadeHeld;
	// Affects the distance of throw
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Explosives")
	float ThrowGrenadeHoldTime;

private:

	// Grenade time before explode
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Explosives", meta = (AllowPrivateAccess = "true"))
	float GrenadeTimeBeforeExplode;

	// Throws grenade
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginThrowGrenade();

	void ThrowGrenade(float ThrowPower);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndThrowGrenade();
	
	// Activate mine server implementation
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMine();

	// Used to delay possibility of using explosives
	UPROPERTY(Replicated)
	bool bIsExplosiveUsed;
	void SetIsExplosiveUsedFalse();
	FTimerHandle ExplosiveUseDelay_Timer;

	// Damage that character takes on falling
	float FallingDamage;
	// True if character was falling last frame
	bool bWasFalling;

protected:

	// Class of spawned weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	TSubclassOf<AWeapon> WeaponClass;

	// Explosives using delay
	UPROPERTY(EditDefaultsOnly, Category = "Explosives")
	float ExplosivesUsingDelay;

	// Grenade class
	UPROPERTY(EditDefaultsOnly, Category = "Ammunition", meta = (BlueprintProtected = "true"))
	TSubclassOf<AExplosives> GrenadeClass;
	// Remote bomb class
	UPROPERTY(EditDefaultsOnly, Category = "Ammunition", meta = (BlueprintProtected = "true"))
	TSubclassOf<AExplosives> RemoteBombClass;


	/**HEALTH LOGICS*/
protected:

	// Method which is called when health changes
	UFUNCTION()
	void ReceiveHealthChanged(UHealthComponent* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser);

public:

	// Defines if character is dead
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bHUDCanBeDestroyed;

private:

	// Function is called on bIsDead change. It is needed to replicate some logics on clients, e.g. SetSimulatePhysics(true)
	UFUNCTION()
	void OnRep_IsDead();

	// Dying logic
	void Dying();

	void Respawn();

public:
	
	// Aim pitch angle
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	float AimPitch;

private:

	// Sets aim pitch offset angle
	void SetAimPitch();
	
	// Corrects weapon accuracy while moving and zooming
	void CorrectAccuracy();

	// Sets grenade's throwing power
	void SetGrenadeThrowingPower();

	// Boosts weapon damage when powerup is picked up
	void BoostDamage();

	// Takes damage on fall from height
	void TakeFallDamage();

	// Toggle zoom
	void Zoom();
};

