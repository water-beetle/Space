// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetNoiseGenerator.h"
#include "GameFramework/Actor.h"
#include "PlanetGenerator.generated.h"

class UPlanetOrbit;
class UGravityFieldCenter;
class UProceduralMeshComponent;
class USplineComponent;

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
	virtual void Tick(float DeltaSeconds) override;

public:	

	// 에디터에서 호출되는 함수
	UFUNCTION(CallInEditor, Category = "Planet Settings")
	void GeneratePlanetInEditor();

	UPROPERTY(VisibleInstanceOnly)
	UProceduralMeshComponent* PlanetMesh;
	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* OceanMesh;
	UPROPERTY(VisibleInstanceOnly)
	UProceduralMeshComponent* OrbitMesh;
	
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

private:
	UPROPERTY(EditAnywhere, Category = "Planet Settings")
	UGravityFieldCenter* GravityField;
	UPROPERTY(EditAnywhere, Category = "Planet Noise Settings")
	FPlanetNoiseGenerator NoiseGenerator;
	UPROPERTY(EditAnywhere, Category = "Planet Orbit Settings")
	UPlanetOrbit* PlanetOrbit;
	
	
	void GeneratePlanet(int32 Resolution, float Radius);
	void GenerateOcean();	
	void GenerateFace(const FVector& LocalUp, int32 Resolution, float Radius, TArray<FVector>& Vertices,
		TArray<int32>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals);
	FVector GetNormalizedPositionOnSphere(const FVector& Position) const;
	
	FVector2D CalculateUV(const FVector2D& GridPosition,int32 Resolution);
	FVector CalculateNormal(const FVector& Vertex);
	
	float MinRadius = FLT_MAX;
	float MaxRadius = FLT_MIN;
	
};
