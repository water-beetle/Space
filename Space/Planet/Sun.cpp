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

void ASun::UpdateSunDirection()
{
	if(!PlayerPawn || !SunLight)
		return;
	
	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	FVector SunPosition = GetActorLocation();  // 예제 태양 위치
	FVector SunDirection = (PlayerLocation - SunPosition).GetSafeNormal();
	
	FRotator TargetRotation = SunDirection.Rotation();
	FRotator CurrentRotation = SunLight->GetActorRotation();

	float InterpSpeed = 1.0f;
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	SunLight->SetActorRotation(NewRotation);
	if (MPCInstance)
	{
		MPCInstance->SetVectorParameterValue(FName("SunDirection"), SunDirection);
	}
		
}
