// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "UnrealMathUtility.h"
#include "UnrealNetwork.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Default armor amount
	Armor = 25.f;
	Health = MaxHealth;
	// Set to replicate this
	SetIsReplicated(true);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind only if owner is server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleOnTakeAnyDamage);
		}
	}
	
}

void UHealthComponent::HandleOnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage == 0)
	{
		return;
	}

	if (Armor == 0)
	{
		Health -= ceil(Damage);
	}
	else
	{
		Health -= floor(Damage / 3);
		float NewArmor = Armor - ceil(2 * Damage / 3);
		if (NewArmor >= 0)
		{
			Armor = NewArmor;
		}
		else
		{
			Health += NewArmor;
			Armor = 0;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Health = %s"), *FString::SanitizeFloat(Health));
	UE_LOG(LogTemp, Warning, TEXT("Armor = %s"), *FString::SanitizeFloat(Armor));
	UE_LOG(LogTemp, Warning, TEXT("Damage = %s"), *FString::SanitizeFloat(Damage));
	// Broadcast on health change delegate to all listeners
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(this, Health, Armor, Damage, DamageType, InstigatedBy, DamageCauser);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Armor);
}


