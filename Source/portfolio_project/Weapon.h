// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class USceneComponent;
class Aportfolio_projectProjectile;
class USoundBase;

// Contains information of a single hit weapon line trace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	// Location where line trace
	UPROPERTY()
	FVector_NetQuantize TraceTo;
	// Type of surface which was hit by trace
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

};

UCLASS()
class PORTFOLIO_PROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()

private:

	// Weapon mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;
	// Muzzle position
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzleOffset;

public:

	//Returns weapon mesh
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	//Returns muzzle offset
	FORCEINLINE USceneComponent* GetMuzzleOffset() const { return MuzzleOffset; }

public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**FIRING FROM WEAPON*/

public:

	// Start weapon fire
	void StartFire();

	// Stop weapon fire
	void StopFire();

	// Muzzle socket name
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

protected:

	// Rocket class
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	TSubclassOf <Aportfolio_projectProjectile> RocketClass;

	// Fires a weapon
	void Fire();

	// Server implementation of Fire()
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	// Weapon shot sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (BlueprintProtected = "true"))
	USoundBase* ShotSound;

	// Rate of fire, shots per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	float FireRate;

	// Start fire timer
	FTimerHandle Fire_Timer;

	// Time of last shot
	float LastShotTime;

	// Time between shots
	float TimeBetweenShots;

	// If true first shot already fired
	bool bFirstShotFired;

	// Muzzle particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	UParticleSystem* MuzzleFireEffect;

	// Impact particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	UParticleSystem* DefaultImpactEffect;

	// Impact particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	UParticleSystem* FleshImpactEffect;

	// Camera shake 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	TSubclassOf<UCameraShake> WeaponCameraShake;

	// Damage dealt by weapon
	float WeaponDamage;

	// Default damage dealt by weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true", DisplayName = "Weapon Damage"))
	float DefaultWeaponDamage;

	// Spread of bullets in degrees
	float BulletSpread;

	// Default spread of bullets in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true", DisplayName = "Bullet Spread"))
	float DefaultBulletSpread;

	// Damage type class reference
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	TSubclassOf<UDamageType> DamageType;

	// HitScanTrace struct
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	void PlayImpactEffect(EPhysicalSurface NewTargetsSurfaceType, FVector NewImpactPoint);

	// Amount of bullets fired in one shot. e.g. assault rifle = 1, shotgun = 12 etc.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true"))
	int32 AmountOfBulletsOnOneShot;

private:

	// Fires shot and defines amount of bullets fired in one shot
	void OneBulletShot();
	// Fires one rocket from rocket launcher
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastOneRocketShot();
	// Bullets fired counter
	int32 BulletsCounter;
	
	// Function is called on HitScanTrace change
	UFUNCTION()
	void OnRep_HitScanTrace();

	// This function calls timer which starts reloading of weapon
	void ReloadDelay();

	// Reloading timer
	FTimerHandle Reload_Timer;

public:

	// Multiply damage dealt
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DamageBoost();

	// Returns default damage back
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DamageBoostEnd();

	// Function that implements reloading logic
	void Reload();

	// Reloading time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadTime;

	// Zoom correction of aim, when zoom aim is better (Lower the value - higher the accuracy)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true", DisplayName = "Zoom Aim Correction"))
	float ZoomAimCorrection;
	// Zoom correction of aim, when zoom aim is better (Higher the value - lower the accuracy)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true", DisplayName = "Movement Aim Correction"))
	float MovementAimCorrection;
	// Both zoom and movement correction of aim, firing while moving and zooming at the same time
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (BlueprintProtected = "true", DisplayName = "Total Aim Correction"))
	float TotalAimCorrection;

	// Default aim correction
	const float DefaultAimCorrection = 1.0f;

};
