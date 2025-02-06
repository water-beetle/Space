// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Space/Data/PlanetData.h"
#include "PlanetNoiseData.generated.h"

/**
 * 
 */
UCLASS()
class SPACE_API UPlanetNoiseData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise Settings")
	FNoiseData NoiseSettings;
};
