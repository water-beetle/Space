#pragma once
#include "PlanetData.generated.h"

USTRUCT(BlueprintType)
struct FOrbitData
{
	GENERATED_BODY()
	
	FOrbitData()
	{
		OrbitRadiusX = 0;
		OrbitRadiusY = 0;
		OrbitSpeed = 0;
		OrbitIncline = FRotator::ZeroRotator;
		RotationSpeed = 0;

		SegmentCount = 100;
		Thickness = 50;
	}

	void SetZero()
	{
		OrbitIncline = FRotator::ZeroRotator;
		OrbitRadiusX = 0.f;
		OrbitRadiusY = 0.f;
		RotationSpeed = 0.f;
	}

	void SetRandom(float MaxRadius, float MinRadius)
	{
		OrbitRadiusX = FMath::RandRange(MinRadius, MaxRadius);
		OrbitRadiusY = FMath::RandRange(MinRadius, MaxRadius);
		OrbitIncline.Yaw = FMath::RandRange(-30.0f, 30.0f);
		OrbitIncline.Pitch = FMath::RandRange(-30.0f, 30.0f);
		OrbitIncline.Roll = FMath::RandRange(-30.0f, 30.0f);
		OrbitSpeed = FMath::RandRange(0.5f, 2.f);
		RotationSpeed = FMath::RandRange(0.5f, 1.5f);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitRadiusX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitRadiusY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	FRotator OrbitIncline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float RotationSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Orbit Visual")
	int32 SegmentCount;
	UPROPERTY(EditAnywhere, Category = "Orbit Visual")
	int Thickness;
};

USTRUCT(BlueprintType)
struct FNoiseData
{
	GENERATED_BODY()

	FNoiseData()
	{

		WarpAmp = 0.6f;
		WarpFrequency = 1.5f;
		
		FBMFrequency = 1.2f;
		FBMOctaves = 8;
		FBMLacunarity = 2.f;
		FBMGain = 0.7f;

		// Ridged
		RidgedFrequency = 1.5f;
		RidgedOctaves = 4;
		RidgedLacunarity = 2.f;
		RidgedGain = 0.5f;
		RidgedOffset = 1.f;

		TerrainThresholdLow = 0.f;
		TerrainThresholdHigh = 1.f;
		DisplacementScale = 1.f;
	}

	void SetZero()
	{
		WarpAmp = 0.0f;
		WarpFrequency = 0.f;
		FBMFrequency = 0.f;
		FBMOctaves = 0;
		FBMLacunarity = 0.f;
		FBMGain = 0.f;

		// Ridged
		RidgedFrequency = 0.f;
		RidgedOctaves = 0;
		RidgedLacunarity = 0.f;
		RidgedGain = 0.f;
		RidgedOffset = 0.f;

		TerrainThresholdLow = 0.f;
		TerrainThresholdHigh = 0.f;
		DisplacementScale = 0.f;
	}
	
	// ---------------- Domain Warp Settings ----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|DomainWarp")
	float WarpAmp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|DomainWarp")
	float WarpFrequency;
	
	// ---------------- FBM 노이즈 파라미터 (평원/대지) ----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|FBM")
	float FBMFrequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|FBM")
	int32 FBMOctaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|FBM")
	float FBMLacunarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|FBM")
	float FBMGain;

	// ---------------- Ridged 노이즈 파라미터 (산맥) ----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Ridged")
	float RidgedFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Ridged")
	int32 RidgedOctaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Ridged")
	float RidgedLacunarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Ridged")
	float RidgedGain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Ridged")
	float RidgedOffset;

	// ---------------- Biome & Blending ----------------
	// FBM과 Ridged를 보간할 때 사용될 임계값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Blend")
	float TerrainThresholdLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise|Blend")
	float TerrainThresholdHigh;

	// 최종 변위 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Planet|Mesh")
	float DisplacementScale;
};


	
