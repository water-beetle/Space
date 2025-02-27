// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"

#include "Space/Planet/Physics/GravityFieldCenter.h"
#include "Space/Planet/Physics/PlanetOrbit.h"
#include "ProceduralMeshComponent.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	RootComponent = PlanetMesh;
	
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

void APlanet::SetPlanetMesh(UStaticMesh* StaticMesh)
{
	if(StaticMesh)
	{
		PlanetMesh->SetStaticMesh(StaticMesh);
	}
}

void APlanet::SetPlanetMaterial(UMaterialInterface* PlanetMaterial)
{
	
	if(PlanetMaterial)
	{
		PlanetMesh->SetMaterial(0, PlanetMaterial);
	}
}

void APlanet::InitPlanet(const FOrbitData& OrbitData, float Radius)
{
	/* 행성을 생성할 때의 기본 설정 함수
	 * 1. 행성의 공전, 자전 설정
	 * 2. 행성의 중력 범위 설정
	 * 3. 행성의 반지름값 저장
	 */
	
	PlanetOrbitComponent->InitOrbit(OrbitData);
	GravityField->SetGravityFieldSize(Radius * 2 + 200);
	PlanetRadius = Radius;
}

void APlanet::AddCharacterToGravityField()
{
	GravityField->AddCharacterToGravityField();
}



