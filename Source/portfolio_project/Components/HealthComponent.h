// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnHealthChangedSignature,
UHealthComponent*, HealthComp, float, Health, float, Armor, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_PROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Handle taken damage
	UFUNCTION()
	void HandleOnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
public:

	// Character's Health which will update during gameplay
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	float Health;

	// Returns maximum health
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	// Character's Armor which will update during gameplay
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Armor")
	float Armor;

	// Returns maximum armor
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FORCEINLINE float GetMaxArmor() const { return MaxArmor; }

	// On health change delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

private:
	// Maximum Health value. Assigns to the character in the beginning of the game
	const float MaxHealth = 100.0f;
	// Maximum Armor value
	const float MaxArmor = 100.0f;
};
