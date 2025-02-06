// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"

#include "GravityFieldCenter.h"
#include "PlanetOrbit.h"
#include "ProceduralMeshComponent.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	RootComponent = PlanetMesh;

	OrbitMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("OrbitMesh"));
	OrbitMesh->SetupAttachment(RootComponent);
	OrbitMesh->SetUsingAbsoluteRotation(true); // 공전 궤도를 나타내기 행성의 위치, 각도에 무관하게 적용
	OrbitMesh->SetUsingAbsoluteLocation(true);
	OrbitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OrbitMesh->CastShadow = false;
	
	GravityField = CreateDefaultSubobject<UGravityFieldCenter>("GravityField");
	GravityField->SetupAttachment(RootComponent);

	PlanetOrbitComponent = CreateDefaultSubobject<UPlanetOrbit>("PlanetOrbit");
	PlanetRadius = 100;
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
}

void APlanet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlanet::SetPlanetMeshAndMaterial(UStaticMesh* StaticMesh, UMaterialInterface* PlanetMaterial)
{
	if(StaticMesh)
	{
		PlanetMesh->SetStaticMesh(StaticMesh);
	}

	if(PlanetMaterial)
	{
		PlanetMesh->SetMaterial(0, PlanetMaterial);
	}
}

void APlanet::AddCharacterToGravityField()
{
	GravityField->AddCharacterToGravityField();
}

void APlanet::InitPlanet(const FOrbitData& OrbitData, const FVector _OrbitCenter, float Radius)
{
	PlanetOrbitComponent->InitOrbit(OrbitData, _OrbitCenter);
	PlanetOrbitComponent->SetOrbitVisualization(OrbitMesh); // 행성의 공전궤도 설정

	// 중력장 범위 설정
	GravityField->SetGravityFieldSize(Radius * 2 + 200);
	PlanetRadius = Radius;
}


