// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Space/Data/PlanetData.h"
#include "Space/ThirdParty/FastNoiseLite.h"
#include "PlanetNoiseGenerator.generated.h"


USTRUCT(BlueprintType)
struct SPACE_API FPlanetNoiseGenerator
{
	GENERATED_BODY()

public:
	FPlanetNoiseGenerator();

	FNoiseData NoiseData;
	
	void InitNoiseParam(FNoiseData _NoiseData);
	float GetHeight(const FVector3f& PointOnSphere);

private:
	// FastNoiseLite 인스턴스
	FastNoiseLite BasicNoiseGenerator;
	FastNoiseLite RigidNoiseGenerator;

	// 노이즈 계산 함수
	float GenerateBaseNoise(const FVector3f& Point) const;
	float GenerateMountainNoise(const FVector3f& Point) const;
};
