// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetNoiseGenerator.h"

#include "Space/Data/PlanetData.h"

FPlanetNoiseGenerator::FPlanetNoiseGenerator()
{
    // FastNoiseLite 초기화
    BasicNoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    BasicNoiseGenerator.SetFrequency(0.01f);
    BasicNoiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
    BasicNoiseGenerator.SetFractalOctaves(5);
    BasicNoiseGenerator.SetFractalLacunarity(2.0f);
    BasicNoiseGenerator.SetFractalGain(0.5f);
    
    RigidNoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    RigidNoiseGenerator.SetFrequency(0.01f);
    RigidNoiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
    RigidNoiseGenerator.SetFractalOctaves(5);
    RigidNoiseGenerator.SetFractalLacunarity(2.0f);
    RigidNoiseGenerator.SetFractalGain(0.5f);
}

// 지형 높이 계산
float FPlanetNoiseGenerator::GetHeight(const FVector3f& PointOnSphere)
{    
	//FVector WarpedPoint = ApplyDomainWarp(PointOnSphere);
	
    // 기본 노이즈
    float BaseHeight = GenerateBaseNoise(PointOnSphere);
    float MountainNoise = GenerateMountainNoise(PointOnSphere);

    // 최종 높이 조합
    float CombinedHeight = BaseHeight + MountainNoise;

    return CombinedHeight;
}

void FPlanetNoiseGenerator::InitNoiseParam(FNoiseData _NoiseData)
{
    this->NoiseData = _NoiseData;
    
    BasicNoiseGenerator.SetFrequency(NoiseData.Frequency);
    BasicNoiseGenerator.SetFractalLacunarity(NoiseData.Lacunarity);
    BasicNoiseGenerator.SetFractalGain(NoiseData.Persistence);
    BasicNoiseGenerator.SetSeed(FMath::Rand());

    RigidNoiseGenerator.SetFrequency(NoiseData.Frequency);
    RigidNoiseGenerator.SetFractalLacunarity(NoiseData.Lacunarity);
    RigidNoiseGenerator.SetFractalGain(NoiseData.Persistence);
    RigidNoiseGenerator.SetSeed(FMath::Rand());
}

// 기본 노이즈
float FPlanetNoiseGenerator::GenerateBaseNoise(const FVector3f& Point) const
{
    float NoiseValue = BasicNoiseGenerator.GetNoise(Point.X * NoiseData.BaseNoiseScale,
        Point.Y * NoiseData.BaseNoiseScale, Point.Z * NoiseData.BaseNoiseScale);
    return NoiseValue * NoiseData.BaseNoiseIntensity;
}

// 산맥 노이즈
float FPlanetNoiseGenerator::GenerateMountainNoise(const FVector3f& Point) const
{
    float NoiseValue = RigidNoiseGenerator.GetNoise(Point.X * NoiseData.MountainNoiseScale,
        Point.Y * NoiseData.MountainNoiseScale, Point.Z * NoiseData.MountainNoiseScale);
    return (1.0f - FMath::Abs(NoiseValue)) * NoiseData.MountainIntensity;
}