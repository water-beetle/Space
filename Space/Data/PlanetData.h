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
		OrbitIncline.Yaw = FMath::RandRange(0.0f, 360.0f);
		OrbitIncline.Pitch = FMath::RandRange(-60.0f, 60.0f);
		OrbitIncline.Roll = FMath::RandRange(0.0f, 360.0f);
		OrbitSpeed = FMath::RandRange(2, 5);
		RotationSpeed = FMath::RandRange(5, 15);
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
		Frequency = 0.01f;
		Lacunarity = 2.0f;
		Persistence = 0.5f;
		BaseNoiseScale = 10.0f;
		BaseNoiseIntensity = 1.0f;
		MountainNoiseScale = 20.0f;
		MountainIntensity = 1.5f;
	}

	void SetZero()
	{
		Frequency = 0.f;
		BaseNoiseIntensity = 0.f;
		MountainIntensity = 0.f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float Frequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float Lacunarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float Persistence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float BaseNoiseScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float BaseNoiseIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float MountainNoiseScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float MountainIntensity;
};


	
