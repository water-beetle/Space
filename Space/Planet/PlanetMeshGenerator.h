// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetNoiseGenerator.h"

struct FNoiseData;
/**
 * 
 */
class SPACE_API PlanetMeshGenerator
{
public:
	PlanetMeshGenerator();
	~PlanetMeshGenerator();
	
	UStaticMesh* GeneratePlanetMesh(const FString& PackagePath, const FString& MeshName, int32 Resolution,
		float Radius, const FNoiseData& NoiseData);

private:
	FPlanetNoiseGenerator NoiseGenerator;
	
	void GenerateMeshData(int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals);
	void GenerateFace(const FVector3f& LocalUp, int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals);
	void FillMeshDescription(FMeshDescription& MeshDescription, const TArray<FVector3f>& Vertices, const TArray<int32>& Triangles,
		const TArray<FVector2f>& UVs, const TArray<FVector3f>& Normals);
	UStaticMesh* CreateStaticMeshAsset(const FString& PackagePath, const FString& MeshName, const FMeshDescription& MeshDescription);
	FVector3f GetNormalizedPositionOnSphere(const FVector3f& Position) const;
	FVector2f CalculateUV(const FVector2f& GridPosition,int32 Resolution);
	FVector3f CalculateNormal(const FVector3f& Vertex, float PlanetRadius);
};
