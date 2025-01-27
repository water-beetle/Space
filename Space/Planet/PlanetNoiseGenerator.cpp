// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetNoiseGenerator.h"

FPlanetNoiseGenerator::FPlanetNoiseGenerator()
: BaseNoiseScale(10.0f)
, BaseNoiseIntensity(1.0f)
, Frequency(0.01)
, Persistence(0.5)
, Lacunarity(2.0)
, MountainNoiseScale(20.0f)
, MountainIntensity(1.5f)
, WarpStrength(50.0f)
, WarpScale(0.05f)
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
    

    WarpNoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    WarpNoiseGenerator.SetFrequency(0.02f);
}

// 지형 높이 계산
float FPlanetNoiseGenerator::GetHeight(const FVector& PointOnSphere)
{    
	//FVector WarpedPoint = ApplyDomainWarp(PointOnSphere);
	
    // 기본 노이즈
    float BaseHeight = GenerateBaseNoise(PointOnSphere);
    float MountainNoise = GenerateMountainNoise(PointOnSphere);

    // 최종 높이 조합
    float CombinedHeight = BaseHeight + MountainNoise;

    return CombinedHeight;
}

void FPlanetNoiseGenerator::InitNoiseParam()
{
    BasicNoiseGenerator.SetFrequency(Frequency);
    BasicNoiseGenerator.SetFractalLacunarity(Lacunarity);
    BasicNoiseGenerator.SetFractalGain(Persistence);
    BasicNoiseGenerator.SetSeed(FMath::Rand());

    RigidNoiseGenerator.SetFrequency(Frequency);
    RigidNoiseGenerator.SetFractalLacunarity(Lacunarity);
    RigidNoiseGenerator.SetFractalGain(Persistence);
    RigidNoiseGenerator.SetSeed(FMath::Rand());
}

// 기본 노이즈
float FPlanetNoiseGenerator::GenerateBaseNoise(const FVector& Point) const
{
    float NoiseValue = BasicNoiseGenerator.GetNoise(Point.X * BaseNoiseScale, Point.Y * BaseNoiseScale, Point.Z * BaseNoiseScale);
    return NoiseValue * BaseNoiseIntensity;
}

// 산맥 노이즈
float FPlanetNoiseGenerator::GenerateMountainNoise(const FVector& Point) const
{
    float NoiseValue = RigidNoiseGenerator.GetNoise(Point.X * MountainNoiseScale, Point.Y * MountainNoiseScale, Point.Z * MountainNoiseScale);
    return (1.0f - FMath::Abs(NoiseValue)) * MountainIntensity;
}


// 도메인 왜곡
FVector FPlanetNoiseGenerator::ApplyDomainWarp(const FVector& Point) const
{
    float WarpX = WarpNoiseGenerator.GetNoise(Point.Y * WarpScale, Point.Z * WarpScale);
    float WarpY = WarpNoiseGenerator.GetNoise(Point.Z * WarpScale, Point.X * WarpScale);
    float WarpZ = WarpNoiseGenerator.GetNoise(Point.X * WarpScale, Point.Y * WarpScale);

    return Point + FVector(WarpX, WarpY, WarpZ) * WarpStrength;
}