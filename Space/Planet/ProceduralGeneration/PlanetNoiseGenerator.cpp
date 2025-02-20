// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetNoiseGenerator.h"

#include "Space/Data/PlanetData.h"

FPlanetNoiseGenerator::FPlanetNoiseGenerator()
{
	PerlinNoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	WarpedNoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	WarpedNoiseGenerator.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
}


FVector3f FPlanetNoiseGenerator::ApplyDomainWarp(const FVector3f& InPos)
{
	float x = InPos.X;
	float y = InPos.Y;
	float z = InPos.Z;
	WarpedNoiseGenerator.DomainWarp(x, y, z);
	return FVector3f(x, y, z);
}

float FPlanetNoiseGenerator::FBMNoise(const FVector3f& Pos, float Frequency, int32 Octaves, float Lacunarity, float Gain)
{
	float Sum = 0.f;
	float Amplitude = 0.5f;
	float TotalAmplitude = 0.f;
	float Freq = Frequency;
	for (int32 i = 0; i < Octaves; i++)
	{
		float n = PerlinNoiseGenerator.GetNoise(Pos.X * Freq, Pos.Y * Freq, Pos.Z * Freq);
		Sum += n * Amplitude;
		TotalAmplitude += Amplitude;
		Amplitude *= Gain;
		Freq *= Lacunarity;
	}
	return (TotalAmplitude > 0.f) ? Sum / TotalAmplitude * 10 : 0.f;
}

float FPlanetNoiseGenerator::RidgedNoise(const FVector3f& Pos, float Frequency, int32 Octaves, float Lacunarity,
    float Gain, float Offset)
{
	float Sum = 0.f;
	float Amplitude = 1.f;
	float Freq = Frequency;
	float Weight = 1.f;
	for (int32 i = 0; i < Octaves; i++)
	{
		float n = PerlinNoiseGenerator.GetNoise(Pos.X * Freq, Pos.Y * Freq, Pos.Z * Freq);
		n = FMath::Abs(n);
		n = Offset - n;
		n = n * n;  // 능선 효과 강화
		n *= Weight;
		Sum += n * Amplitude;
		Weight = FMath::Clamp(n * Gain, 0.f, 1.f);
		Freq *= Lacunarity;
		Amplitude *= Gain;
	}
	return Sum;
}

float FPlanetNoiseGenerator::CraterField(const FVector3f& NormalizedPos, const TArray<FCraterData>& Craters)
{
	float TotalOffset = 0.f;
	for (const FCraterData& Crater : Craters)
	{
		float Dist = FVector3f::Dist(NormalizedPos, Crater.Center);
		if (Dist < Crater.Radius)
		{
			// 중심에서 멀어질수록 깊이가 작아지도록 (포물선 형태)
			float Alpha = Dist / Crater.Radius;
			float DepthValue = Crater.Depth * (1.f - Alpha * Alpha);
			TotalOffset -= DepthValue;
		}
	}
	return TotalOffset;
}

float CraterField(const FVector3f& NormalizedPos, const TArray<FCraterData>& Craters)
{
	float TotalOffset = 0.f;
	for (const FCraterData& Crater : Craters)
	{
		float Dist = FVector3f::Dist(NormalizedPos, Crater.Center);
		if (Dist < Crater.Radius)
		{
			// 중심에서 멀어질수록 깊이가 작아지도록 (포물선 형태)
			float Alpha = Dist / Crater.Radius;
			float DepthValue = Crater.Depth * (1.f - Alpha * Alpha);
			TotalOffset -= DepthValue;
		}
	}
	return TotalOffset;
}

float FPlanetNoiseGenerator::GenerateNoise(const FVector3f& NormalizedPos, const FNoiseData& NoiseData)
{
	WarpedNoiseGenerator.SetDomainWarpAmp(NoiseData.WarpAmp);
	FVector3f WarpedPos = ApplyDomainWarp(NormalizedPos);

    // 2) FBM & Ridged
    float baseVal = FBMNoise(WarpedPos, NoiseData.FBMFrequency, NoiseData.FBMOctaves, NoiseData.FBMLacunarity, NoiseData.FBMGain);
    float ridgeVal = RidgedNoise(WarpedPos, NoiseData.RidgedFrequency, NoiseData.RidgedOctaves, NoiseData.RidgedLacunarity, NoiseData.RidgedGain, NoiseData.RidgedOffset);

    // 3) 보간 (baseVal이 특정 범위 이상이면 ridgeVal 쪽으로)
    float blendT = SmoothStep(NoiseData.TerrainThresholdLow, NoiseData.TerrainThresholdHigh, baseVal);
    float combined = FMath::Lerp(baseVal, ridgeVal, blendT);
	float highFreqVal = FBMNoise(WarpedPos, 50.f, 2, 2.f, 0.5f);
	combined += highFreqVal * 0.1f;
    // 4) 크레이터 (옵션)
    //float crater = CraterField(NormalizedPos, Craters);

    // 5) 최종 변위 = (노이즈 * DisplacementScale) + (크레이터 * DisplacementScale)
    return combined * NoiseData.DisplacementScale; // + crater * DisplacementScale
}

float FPlanetNoiseGenerator::SmoothStep(float edge0, float edge1, float x)
{
	float T = FMath::Clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return T * T * (3.f - 2.f * T);
}
