// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetMeshGenerator.h"
#include "GameFramework/Actor.h"
#include "Space/Data/PlanetData.h"
#include "SolarSystem.generated.h"

class UPlanetNoiseData;
class ADirectionalLight;
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
	float MinOrbitRadius;	
	UPROPERTY(EditAnywhere, Category="Solar System")
	float MaxOrbitRadius;
	UPROPERTY(EditAnywhere, Category="Solar System")
	int32 PlanetNums;
	
	
	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	FVector SunPosition;
	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	float SunRadius;
	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	int32 SunResolution;
	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	UMaterialInterface* SunMaterial;
	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	ADirectionalLight* DirectionalSunLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Solar System|Planet")
	UPlanetNoiseData* NoiseDataAsset;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	bool bUseNewPlanetMesh;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetMinRadius;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetMaxRadius;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetResolution;
	UPROPERTY(EditAnywhere, Category = "Solar System|Planet")
	TArray<UMaterialInterface*> PlanetMaterialArray;
	
	FString PlanetSaveDirectory = "/Game/Meshes/Planet/";
	FString PlanetMeshName = "Planet_";

private:
	void SetRandomPlanetData();
	void GeneratePlanetMesh();
	void GenerateSun();
	void GeneratePlanets();
	void PlaceMainCharacter();
	void SetMovePlanet();

	PlanetMeshGenerator MeshGenerator;

	ASun* Sun;
	APawn* PlayerPawn;
	FNoiseData SunNoiseData;
	FOrbitData SunOrbitData;

	UPROPERTY()
	TArray<FPlanetData> PlanetDataArray;
	
};
