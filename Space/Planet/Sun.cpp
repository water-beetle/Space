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

void ASun::InitSun(const FOrbitData& OrbitData, float Radius, ADirectionalLight* DirectionalLight)
{
	/* 태양을 생성할 때의 기본 설정 함수
	 * 1. 부모의 InitPlanet 함수 실행
	 * 2. Mesh의 Light Channel 설정 -> 태양을 표현하는 Directional Light가 태양에게 가리지 않도록
	 * 3. Directional Light 저장
	 */
	
	Super::InitPlanet(OrbitData, Radius);

	FLightingChannels& LightChannels = PlanetMesh->LightingChannels;
	LightChannels.bChannel0 = false;
	PlanetMesh->MarkRenderStateDirty();

	DirectionalSunLight = DirectionalLight;
}

void ASun::UpdateSunDirection()
{
	/* 태양빛을 처리하는 함수
	 * 1. 플레이어와 태양의 위치를 계산해 Directional Light 방향을 업데이트
	 * 2. Material에서 태양 빛 반사 처리를 할 수 있도록 MPCInstance의 SunDirection에 값을 설정
	 */
	
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
