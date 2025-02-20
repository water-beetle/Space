// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetNoiseGenerator.h"
#include "PlanetGenerator.generated.h"

class APlanet;
struct FNoiseData;
/**
 * 
 */
UCLASS()
class SPACE_API UPlanetGenerator : public UObject
{

	GENERATED_BODY()
	
public:
	UPlanetGenerator();
	
	APlanet* GeneratePlanet(FString PlanetName, int32 Resolution, float Radius, const FNoiseData& NoiseData);

private:
	FPlanetNoiseGenerator NoiseGenerator;

	/*
	 * Save Bluepriunt
	 */
	FString PlanetPackagePath = "/Game/Meshes/Planet/";
	
	/*
	 * Planet Procedural Static Mesh
	 */
	UStaticMesh* GeneratePlanetMesh(const FString& PackagePath, const FString& MeshName, int32 Resolution,
		float Radius, const FNoiseData& NoiseData);
	void GenerateMeshData(int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals, const FNoiseData& NoiseData);
	void GenerateFace(const FVector3f& LocalUp, int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals, const FNoiseData& NoiseData);
	void FillMeshDescription(FMeshDescription& MeshDescription, const TArray<FVector3f>& Vertices, const TArray<int32>& Triangles,
		const TArray<FVector2f>& UVs, const TArray<FVector3f>& Normals);
	UStaticMesh* CreateStaticMeshAsset(const FString& PackagePath, const FString& MeshName, const FMeshDescription& MeshDescription);
	FVector3f GetNormalizedPositionOnSphere(const FVector3f& Position) const;
	FVector2f CalculateUV(const FVector2f& GridPosition,int32 Resolution);
	//FVector3f CalculateNormal(const FVector3f& Vertex, float PlanetRadius);

	/*
	 * Planet Procedural Foliage
	 */
};
