// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Space/Data/PlanetData.h"
#include "Space/ThirdParty/FastNoiseLite.h"
#include "PlanetNoiseGenerator.generated.h"

struct FCraterData
{
	// 단위구 상의 중심 좌표 (예: (0, 0, 1) → +Z 방향)
	FVector3f Center = FVector3f(0.f, 0.f, 1.f);
	// 크레이터 반경 (0~1 범위)
	float Radius = 0.05f;
	// 크레이터 깊이 (음의 변위 크기)
	float Depth = 0.02f;

	FCraterData() {}
	FCraterData(const FVector3f& InCenter, float InRadius, float InDepth)
		: Center(InCenter), Radius(InRadius), Depth(InDepth)
	{}
};

USTRUCT(BlueprintType)
struct SPACE_API FPlanetNoiseGenerator
{
	GENERATED_BODY()

public:
	FPlanetNoiseGenerator();

	float GenerateNoise(const FVector3f& NormalizedPos, const FNoiseData& NoiseData);

private:
	FastNoiseLite PerlinNoiseGenerator;
	FastNoiseLite WarpedNoiseGenerator;

	// 노이즈 함수들
	FVector3f ApplyDomainWarp(const FVector3f& InPos);
	float FBMNoise(const FVector3f& Pos, float Frequency, int32 Octaves, float Lacunarity, float Gain);
	float RidgedNoise(const FVector3f& Pos, float Frequency, int32 Octaves, float Lacunarity, float Gain, float Offset);
	float CraterField(const FVector3f& NormalizedPos, const TArray<FCraterData>& Craters);
	
	float SmoothStep(float edge0, float edge1, float x);
};
