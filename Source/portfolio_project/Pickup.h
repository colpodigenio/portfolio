// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class URotatingMovementComponent;
class USkeletalMeshComponent;
class USceneComponent;
class AFPSCharacter;
class USoundBase;

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	SupplyPickup,
	PowerupOnTime,
	PowerupOnTick,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8 
{
	None,
	AssaultRifle,
	Shotgun,
	RocketLauncher,
};

UENUM(BlueprintType)
enum class ESupplyType : uint8
{
	None,
	AssaultRifleAmmo,
	ShotgunAmmo,
	RocketLauncherAmmo,
	Grenade,
	Mine,
	Meds,
	Armor,
};

UENUM(BlueprintType)
enum class EPowerupType : uint8
{
	None,
	SpeedBoost,
	DamageBoost,
	Regeneration
};
UCLASS()
class PORTFOLIO_PROJECT_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Weapon class instance
	UPROPERTY(BlueprintReadWrite, Category = "Pickups")
	USkeletalMeshComponent* PickupWeapon;

	// Static mesh represents pickup
	UPROPERTY(BlueprintReadWrite, Category = "Pickups")
	UStaticMeshComponent* PickupSupply;

	// Point light will highlight pickup
	UPROPERTY(VisibleAnywhere, Category = "Pickups")
	UPointLightComponent* Light;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	// Scene component to be root
	UPROPERTY(VisibleAnywhere, Category = "Pickups", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	// Will rotate actor around z axis
	UPROPERTY(VisibleDefaultsOnly, Category = "Pickups", meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotateComponent;

public:
	
	// Event is called when player takes a pick up 
	void OnPickedup();

	// Pickup type to choose for specific object
	UPROPERTY(EditDefaultsOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	EPickupType PickupType;
	// Weapon type to choose for specific wepon pickup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	EWeaponType WeaponType;
	// Supply type to choose for specific supply
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	ESupplyType SupplyType;
	// Powerup type to choose for specific powerup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	EPowerupType PowerupType;

	// Character which activated powerup
	UPROPERTY(BlueprintReadWrite, Category = "Pickup")
	AFPSCharacter* PowerupUser;

	/**SUPPLY PICK UP*/

	// Event triggers after picking up supply(ammo, meds, etc.)
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (BlueprintProtected = "true"))
	void OnSupplyPickup();

	// Defines the amount of ammo on which will increase player's amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	int32 NumberOfMagazines;

	// Meds delta depends on type small, large. Default value is for small
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition", meta = (BlueprintProtected = "true"))
	float DeltaHealth;

	// Armor delta depends on type small, large. Default value is for small
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammunition", meta = (BlueprintProtected = "true"))
	float DeltaArmor;

	// Health regenerating on tick
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	float RegeneratingHealthOnTick;

	/**POWER UP ON TIME*/
	// Event triggers after picking up power up which works on time
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (BlueprintProtected = "true"))
	void OnPowerupOnTimeActivated();

	// Event triggers after on time power up expired
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (BlueprintProtected = "true"))
	void OnPowerupOnTimeExpired();

	// Power up on time duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	float PowerupDuration;
	FTimerHandle PowerupDurationTimer;

	/**POWER UP ON TICK*/
	// Event triggers after picking up power up on tick
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (BlueprintProtected = "true"))
	void OnPowerupOnTick();

	// Number of ticks which event will be called
	UPROPERTY(EditDefaultsOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	int32 NumberOfTicks;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup", meta = (BlueprintProtected = "true"))
	float TimeBetweenTicks;

	FTimerHandle TickTimer;
	// Increments every tick till it become equal to NumberOfTicks
	int32 TickCounter;
	// Recursive function that will call OnPowerupOnTick event every tick
	void TickTrigger();
};
