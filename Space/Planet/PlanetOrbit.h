// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Components/ActorComponent.h"
#include "Space/Data/PlanetData.h"
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void InitOrbit(const FOrbitData& OrbitData, const FVector _OrbitCenter); // 궤도에 맞는 행성의 초기 위치 설정
	void SetOrbitVisualization(UProceduralMeshComponent* OrbitMesh); // 궤도 시각화 초기화

private:
	void GenerateEllipseMesh(UProceduralMeshComponent* OrbitMesh); // 타원 궤도 생성
	void PlanetRevolve(float DeltaTime); // 공전처리
	void PlanetRotation(float DeltaTime); // 자전처리
	FVector CalculateOrbitPointsByTheta(float Theta, float RadiusX, float RadiusY) const;
	float CalculateEllipseCircumference(float RadiusX, float RadiusY) const; // 타원 둘레 근사


	float OrbitRadiusX;
	float OrbitRadiusY;
	float OrbitSpeed;
	FRotator OrbitIncline;
	FVector OrbitCenter;
	float RotationSpeed;
	int32 SegmentCount;
	int Thickness;

	float AngularSpeed; // 각속도
	float CurrentOrbitTheta; // 현재 공전 각도
	float CurrentRotationTheta; // 현재 자전 각도
};
