// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetMeshGenerator.h"
#include "GameFramework/Actor.h"
#include "Space/Data/PlanetData.h"
#include "SolarSystem.generated.h"

class APlanet;
class ASun;

USTRUCT()
struct FPlanetData
{
	GENERATED_BODY()

	FOrbitData OrbitData;
	FNoiseData NoiseData;
	float Radius;
	
	UPROPERTY()
	APlanet* Planet;
};

UCLASS()
class SPACE_API ASolarSystem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASolarSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	
	UPROPERTY(EditAnywhere, Category="Solar System")
	float MinRadius;	
	UPROPERTY(EditAnywhere, Category="Solar System")
	float MaxRadius;
	
	UPROPERTY(EditAnywhere, Category="Sun")
	FVector SunPosition;
	UPROPERTY(EditAnywhere, Category="Sun")
	float SunRadius;
	UPROPERTY(EditAnywhere, Category="Sun")
	UMaterialInterface* SunMaterial;

	UPROPERTY(EditAnywhere, Category="Planet")
	bool bUseNewPlanetMesh;
	UPROPERTY(EditAnywhere, Category="Planet")
	int32 PlanetNums;
	UPROPERTY(EditAnywhere, Category = "Planet")
	TArray<UMaterialInterface*> PlanetMaterialArray;
	
	FString PlanetSaveDirectory = "/Game/Meshes/Planet/";
	FString PlanetMeshName = "Planet_";

private:
	void SetRandomPlanetData();
	void GeneratePlanetMesh();
	void GenerateSun();
	void GeneratePlanets();
	
	PlanetMeshGenerator MeshGenerator;

	ASun* Sun;
	FNoiseData SunNoiseData;
	FOrbitData SunOrbitData;

	UPROPERTY()
	TArray<FPlanetData> PlanetDataArray;
	
};
