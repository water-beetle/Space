// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeshDescription.h"
#include "PlanetNoiseGenerator.h"
#include "PlanetGenerator.generated.h"

class APlanet;
struct FNoiseData;
/**
 * 
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACE_API UPlanetGenerator : public UActorComponent
{

	GENERATED_BODY()
	
public:
	UPlanetGenerator();

	UPROPERTY(EditAnywhere, Category="Solar System|Sun")
	UMaterialInterface* SunMaterial;
	UPROPERTY(EditAnywhere, Category = "Solar System|Planet")
	TArray<UMaterialInterface*> PlanetMaterialArray;
	
	APlanet* GeneratePlanet(const FString& PlanetName, int32 Resolution, float Radius, const FNoiseData& NoiseData, bool IsSun);

private:
	FPlanetNoiseGenerator NoiseGenerator;

	/*
	 * Save Bluepriunt
	 */
	FString PlanetPackagePath = TEXT("/Game/Meshes/Planet/");
	
	/*
	 * Planet Procedural Static Mesh
	 */
	UStaticMesh* GeneratePlanetMesh(const FString& MeshName, int32 Resolution,
		float Radius, const FNoiseData& NoiseData);
	void GenerateMeshData(int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals, const FNoiseData& NoiseData);
	void GenerateFace(const FVector3f& LocalUp, int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals, const FNoiseData& NoiseData);
	void FillMeshDescription(FMeshDescription& MeshDescription, const TArray<FVector3f>& Vertices, const TArray<int32>& Triangles,
		const TArray<FVector2f>& UVs, const TArray<FVector3f>& Normals);
	UStaticMesh* CreateStaticMeshAsset(const FString& MeshName, const FMeshDescription& MeshDescription);
	FVector3f GetNormalizedPositionOnSphere(const FVector3f& Position) const;
	FVector2f CalculateUV(const FVector2f& GridPosition,int32 Resolution);
	//FVector3f CalculateNormal(const FVector3f& Vertex, float PlanetRadius);

	/*
	 * Planet Procedural Foliage
	 */
};
