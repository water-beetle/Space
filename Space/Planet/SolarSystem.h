// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Space/Planet/ProceduralGeneration/PlanetGenerator.h"
#include "GameFramework/Actor.h"
#include "Space/Data/PlanetData.h"
#include "SolarSystem.generated.h"

class UPlanetNoiseData;
class ADirectionalLight;
class APlanet;
class ASun;
class ASpaceCharacter;

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
	ADirectionalLight* DirectionalSunLight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Solar System|Planet")
	UPlanetNoiseData* NoiseDataAsset; // 다양한 행성 생성을 위해 배열로 선언해야 되나..?, 일단은 하나로
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetMinRadius;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetMaxRadius;
	UPROPERTY(EditAnywhere, Category="Solar System|Planet")
	int32 PlanetResolution;

	UPROPERTY(EditDefaultsOnly, Category="Solar System|Generator")
	UPlanetGenerator* PlanetGenerator;
	
	FString PlanetSaveDirectory = "/Game/Meshes/Planet/";
	FString PlanetMeshName = "Planet_";

	void GenerateSolarSystem();

private:
	void GenerateSun();
	void GeneratePlanets();
	
	void PlaceMainCharacter();
	void TryPlaceCharacter();
	void SetMovePlanet();

	UPROPERTY()
	TSubclassOf<ACharacter> FunnyRobotBPClass;
	UPROPERTY()
	ASpaceCharacter* SpaceCharacterPlayer;
	FNoiseData SunNoiseData;
	FOrbitData SunOrbitData;

	UPROPERTY()
	ASun* Sun;
	UPROPERTY()
	TArray<APlanet*> PlanetArray;
	
	// UPROPERTY()
	// TArray<FPlanetData> PlanetDataArray;
	
};
