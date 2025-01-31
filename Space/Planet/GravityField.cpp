// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityField.h"
#include "Components/SphereComponent.h"
#include "Space/Characters/GravityBody.h"
#include "Space/Characters/SpaceCharacter.h"


// Sets default values
UGravityField::UGravityField()
{
	// 이벤트 바인딩
	OnComponentBeginOverlap.AddDynamic(this, &UGravityField::OnEnterGravityArea);
	OnComponentEndOverlap.AddDynamic(this, &UGravityField::OnExitGravityArea);

	GravityScale = 1.0f;
}

// Called when the game starts or when spawned
void UGravityField::BeginPlay()
{
	Super::BeginPlay();

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	// BeginPlay에서 Overlap된 Actor 찾으려니까 안찾아짐
	// BeginPlay 이후 0.1초 지연 후 초기화 함수 호출
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGravityField::DelayedInitialize);
}

void UGravityField::SetGravityFieldSize(float Radius)
{
	SetSphereRadius(Radius, true); // 반지름을 500으로 설정
}

void UGravityField::OnEnterGravityArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ASpaceCharacter::StaticClass()))
	{
		ASpaceCharacter* SpaceCharacter = Cast<ASpaceCharacter>(OtherActor);
		if (SpaceCharacter)
		{
			SpaceCharacter-> GetGravityBody()->AddGravityArea(this);
		}
	}
}

void UGravityField::OnExitGravityArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ASpaceCharacter::StaticClass()))
	{
		ASpaceCharacter* SpaceCharacter = Cast<ASpaceCharacter>(OtherActor);
		if (SpaceCharacter)
		{
			SpaceCharacter-> GetGravityBody()->RemoveGravityArea(this);
		}
	}
}

void UGravityField::DelayedInitialize()
{
	TArray<AActor*> OverlappingActors;
	GetOwner()->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor && OverlappingActor != GetOwner()) // 자기 자신 제외
		{
			// 특정 클래스 타입(ASpaceCharacter)인지 확인
			if (OverlappingActor->IsA(ASpaceCharacter::StaticClass()))
			{
				ASpaceCharacter* SpaceCharacter = Cast<ASpaceCharacter>(OverlappingActor);
				if (SpaceCharacter)
				{
					SpaceCharacter-> GetGravityBody()->AddGravityArea(this);
				}
			}
		}
	}
}

