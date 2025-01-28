// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetNoiseGenerator.h"
#include "GameFramework/Actor.h"
#include "PlanetGenerator.generated.h"

class UGravityFieldCenter;
class UProceduralMeshComponent;

UCLASS()
class SPACE_API APlanetGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanetGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 에디터에서 호출되는 함수
	UFUNCTION(CallInEditor, Category = "Planet Settings")
	void GeneratePlanetInEditor();

	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	UMaterialInterface* PlanetMaterial;
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	UMaterialInterface* WaterMaterial;
	
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	int32 PlanetResolution;
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	float PlanetRadius;
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	float WaterRadius;

	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	bool bDrawDebugPoint = false;
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	bool bCreateOcean = false;	

	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	FPlanetNoiseGenerator NoiseGenerator;

	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	UGravityFieldCenter* GravityField;

private:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* PlanetMesh;
	UPROPERTY(VisibleAnywhere, Category = "Procedural Mesh")
	UStaticMeshComponent* OceanMesh;

	float MinRadius = FLT_MAX;
	float MaxRadius = FLT_MIN;

	void GeneratePlanet(int32 Resolution, float Radius);
	void GenerateOcean();	
	void GenerateFace(const FVector& LocalUp, int32 Resolution, float Radius, TArray<FVector>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals);
	FVector GetNormalizedPositionOnSphere(const FVector& Position) const;
	
	FVector2D CalculateUV(const FVector2D& GridPosition,int32 Resolution);
	FVector CalculateNormal(const FVector& Vertex);
	
	
};
