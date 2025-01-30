// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Components/ActorComponent.h"
#include "PlanetOrbit.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACE_API UPlanetOrbit : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlanetOrbit();

	UPROPERTY(EditAnywhere, Category = "Orbit")
	UMaterialInterface* OrbitMaterial;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	bool bUseRandomOrbit = false;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float OrbitRadiusX;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float OrbitRadiusY;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float OrbitSpeed;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	FRotator OrbitIncline;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	FVector OrbitCenter;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	int32 SegmentCount;
	UPROPERTY(EditAnywhere, Category = "Orbit")
	int Thickness;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SetOrbitVisualization(UProceduralMeshComponent* OrbitMesh); // 궤도 시각화 초기화

private:
	void InitPlanetPosition(); // 궤도에 맞는 행성의 초기 위치 설정
	void GenerateEllipseMesh(UProceduralMeshComponent* OrbitMesh); // 타원 궤도 생성
	void PlanetRevolve(float DeltaTime); // 공전처리
	void PlanetRotation(float DeltaTime); // 자전처리
	FVector CalculateOrbitPointsByTheta(float Theta, float RadiusX, float RadiusY) const;
	
	float CurrentOrbitTheta; // 현재 공전 각도
	float CurrentRotationTheta; // 현재 자전 각도
};
