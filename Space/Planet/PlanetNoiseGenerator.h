// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Space/ThirdParty/FastNoiseLite.h"
#include "PlanetNoiseGenerator.generated.h"

USTRUCT(BlueprintType)
struct SPACE_API FPlanetNoiseGenerator
{
	GENERATED_BODY()

public:
	FPlanetNoiseGenerator();

	void InitNoiseParam();

	// 노이즈 기반으로 지형 높이 계산
	float GetHeight(const FVector& PointOnSphere);

	// 지형 요소별 파라미터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Noise")
	float BaseNoiseScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Noise")
	float BaseNoiseIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Noise")
	float Frequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Noise")
	float Lacunarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Noise")
	float Persistence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mountains")
	float MountainNoiseScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mountains")
	float MountainIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warping")
	float WarpStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warping")
	float WarpScale;

private:
	// FastNoiseLite 인스턴스
	FastNoiseLite BasicNoiseGenerator;
	FastNoiseLite RigidNoiseGenerator;
	FastNoiseLite WarpNoiseGenerator;

	// 크레이터 위치 저장
	TArray<FVector> CraterCenters;

	// 노이즈 계산 함수
	float GenerateBaseNoise(const FVector& Point) const;
	float GenerateMountainNoise(const FVector& Point) const;
	FVector ApplyDomainWarp(const FVector& Point) const;
};
