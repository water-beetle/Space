// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "Sun.h"


// Sets default values
ASolarSystem::ASolarSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MinRadius = 10000;
	MaxRadius = 100000;
	
	SunPosition = FVector::ZeroVector;
	SunRadius = 5000;
	SunNoiseData.SetZero();
	SunOrbitData.SetZero();
	
	PlanetNums = 10;
	bUseNewPlanetMesh = false;
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();

	SetRandomPlanetData();
	GeneratePlanetMesh();
	GenerateSun();
	GeneratePlanets();
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASolarSystem::SetRandomPlanetData()
{
	for (int i = 0; i < PlanetNums; ++i)
	{
		FPlanetData PlanetData;
		PlanetData.Radius = FMath::RandRange(10000, 15000);
		PlanetData.OrbitData.SetRandom(MinRadius, MaxRadius);
		PlanetDataArray.Add(PlanetData);
	}
}

void ASolarSystem::GeneratePlanetMesh()
{
	FString SunPath = FPackageName::LongPackageNameToFilename(PlanetSaveDirectory + "Sun",
		FPackageName::GetAssetPackageExtension());

	if(!IFileManager::Get().FileExists(*SunPath))
	{
		MeshGenerator.GeneratePlanetMesh(PlanetSaveDirectory, "Sun", 100, SunRadius, SunNoiseData);
	}
	
	for (int i = 0; i < PlanetNums; ++i)
	{
		FString PlanetPath = FPackageName::LongPackageNameToFilename(PlanetSaveDirectory + PlanetMeshName +
			FString::FromInt(i), FPackageName::GetAssetPackageExtension());

		if(bUseNewPlanetMesh || !IFileManager::Get().FileExists(*PlanetPath))
		{
			MeshGenerator.GeneratePlanetMesh(PlanetSaveDirectory, PlanetMeshName + FString::FromInt(i),
				50, 1000, PlanetDataArray[i].NoiseData);
		}
	}
}

void ASolarSystem::GenerateSun()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	Sun = GetWorld()->SpawnActor<ASun>(ASun::StaticClass(), SunPosition, FRotator::ZeroRotator, SpawnParams);
	Sun->InitPlanet(SunOrbitData, SunPosition, SunRadius);
	Sun->SetActorLocation(SunPosition);
	
	FString SunPath = PlanetSaveDirectory + TEXT("Sun");
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *SunPath);
	Sun->SetPlanetMeshAndMaterial(Mesh, SunMaterial);
}

void ASolarSystem::GeneratePlanets()
{
	for (int i = 0; i < PlanetNums; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
	
		APlanet* Planet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParams);
		Planet->InitPlanet(PlanetDataArray[i].OrbitData, SunPosition, PlanetDataArray[i].Radius);
	
		FString PlanetPath = PlanetSaveDirectory + PlanetMeshName + FString::FromInt(i);;
		UStaticMesh* PlanetMesh = LoadObject<UStaticMesh>(nullptr, *PlanetPath);
		UMaterialInterface* PlanetMaterial = nullptr;
		if(PlanetMaterialArray.Num() > 0)
		{
			PlanetMaterial = PlanetMaterialArray[FMath::RandRange(0, PlanetMaterialArray.Num())];
		}
		
		Planet->SetPlanetMeshAndMaterial(PlanetMesh, PlanetMaterial);

		PlanetDataArray[i].Planet = Planet;
	}
} 

