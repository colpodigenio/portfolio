// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "portfolio_projectHUD.generated.h"

UCLASS()
class Aportfolio_projectHUD : public AHUD
{
	GENERATED_BODY()

public:
	Aportfolio_projectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;


};

