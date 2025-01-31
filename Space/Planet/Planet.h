// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetNoiseGenerator.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

class UProceduralMeshComponent;
class UPlanetOrbit;
class UGravityFieldCenter;

UCLASS()
class SPACE_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties	
	APlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:	
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	UGravityFieldCenter* GravityField;
	
	UPROPERTY(EditAnywhere, Category = "Planet")
	float PlanetRadius;

	void SetPlanetMeshAndMaterial(UStaticMesh* StaticMesh, UMaterialInterface* PlanetMaterial);
	void InitPlanet(const FOrbitData& OrbitData, const FVector _OrbitCenter, float Radius);

private:
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	UStaticMeshComponent* PlanetMesh;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	UProceduralMeshComponent* OrbitMesh;
	
	UPROPERTY(EditAnywhere, Category = "Planet")
	UPlanetOrbit* PlanetOrbitComponent;
};
