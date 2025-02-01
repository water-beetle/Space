// Fill out your copyright notice in the Description page of Project Settings.


#include "Sun.h"

#include "Engine/DirectionalLight.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

ASun::ASun()
{
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> MPCFinder(TEXT("/Game/Materials/Sun/MPC_SunDirection"));
	if (MPCFinder.Succeeded())
	{
		SunDirectionMPC = MPCFinder.Object;
	}
}

void ASun::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PlayerPawn = PC->GetPawn();
	}
	MPCInstance = GetWorld()->GetParameterCollectionInstance(SunDirectionMPC);
}

void ASun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSunDirection();
}

void ASun::InitPlanet(const FOrbitData& OrbitData, const FVector _OrbitCenter, float Radius)
{
	Super::InitPlanet(OrbitData, _OrbitCenter, Radius);

	FLightingChannels& LightChannels = PlanetMesh->LightingChannels;
	LightChannels.bChannel0 = false;
	PlanetMesh->MarkRenderStateDirty();
}

void ASun::UpdateSunDirection()
{
	if(!PlayerPawn || !DirectionalSunLight)
		return;
	
	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	FVector SunPosition = GetActorLocation();
	FVector SunDirection = (PlayerLocation - SunPosition).GetSafeNormal();
	
	FRotator TargetRotation = SunDirection.Rotation();
	FRotator CurrentRotation = DirectionalSunLight->GetActorRotation();

	float InterpSpeed = 1.0f;
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	DirectionalSunLight->SetActorRotation(NewRotation);
	if (MPCInstance)
	{
		MPCInstance->SetVectorParameterValue(FName("SunDirection"), SunDirection);
	}
		
}
