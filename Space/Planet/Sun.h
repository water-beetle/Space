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

	virtual void InitPlanet(const FOrbitData& OrbitData, const FVector _OrbitCenter, float Radius) override;
	
	FORCEINLINE void SetDiretionalLight(ADirectionalLight* DirectionalLight){DirectionalSunLight = DirectionalLight;}
private:
	APawn* PlayerPawn;
	UMaterialParameterCollectionInstance* MPCInstance;
	
	void UpdateSunDirection();
};
