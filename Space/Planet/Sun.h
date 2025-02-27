// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Space/Planet/Planet.h"
#include "Sun.generated.h"

class ADirectionalLight;
class UMaterialParameterCollectionInstance;
/**
 * 
 */
UCLASS()
class SPACE_API ASun : public APlanet
{
	GENERATED_BODY()

public:
	ASun();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "MPC")
	UMaterialParameterCollection* SunDirectionMPC;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	ADirectionalLight* DirectionalSunLight;

	void InitSun(const FOrbitData& OrbitData, float Radius, ADirectionalLight* DirectionalLight);
	
private:
	APawn* PlayerPawn;
	UMaterialParameterCollectionInstance* MPCInstance;
	
	void UpdateSunDirection();
};
