// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Space/Planet/PlanetGenerator.h"
#include "SunGenerator.generated.h"

class ADirectionalLight;
class UMaterialParameterCollectionInstance;
/**
 * 
 */
UCLASS()
class SPACE_API ASunGenerator : public APlanetGenerator
{
	GENERATED_BODY()

public:
	ASunGenerator();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "MPC")
	UMaterialParameterCollection* SunDirectionMPC;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	ADirectionalLight* SunLight;
	
private:
	APawn* PlayerPawn;
	UMaterialParameterCollectionInstance* MPCInstance;
	
	void UpdateSunDirection();
};
