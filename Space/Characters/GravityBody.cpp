// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBody.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Space/Planet/GravityField.h"


// Sets default values for this component's properties
UGravityBody::UGravityBody()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UGravityBody::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGravityBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGravityBody::AddGravityArea(UGravityField* GravityField)
{
	GravityFields.Add(GravityField);
	GravityFields.Sort([](const UGravityField& A, const UGravityField& B)
	{
		return A.Priority < B.Priority;
	});

	if(ACharacter* GravityCharacter = Cast<ACharacter>(GetOwner()))
	{
		GravityCharacter->GetCharacterMovement()->GravityScale = GravityFields.Last()->GravityScale;
	}
}

void UGravityBody::RemoveGravityArea(UGravityField* GravityField)
{
	GravityFields.Remove(GravityField);
	GravityFields.Sort([](const UGravityField& A, const UGravityField& B)
	{
		return A.Priority < B.Priority;
	});

	if(ACharacter* GravityCharacter = Cast<ACharacter>(GetOwner()))
	{
		if(GravityFields.Num() == 0)
		{
			GravityCharacter->GetCharacterMovement()->GravityScale = .0f;
		}
		else
		{
			GravityCharacter->GetCharacterMovement()->GravityScale = GravityFields.Last()->GravityScale;
		}
	}
}

FVector UGravityBody::GetGravityDirection()
{
	// GravityFields가 0일 때는 Gravity Scale이 0이라 아마 영향 없을듯
	// Zero Vector로 설정하니까 set gravity에서 ensure에 걸림
	if (GravityFields.Num() == 0) return FVector::ForwardVector;

	// 가장 우선순위 높은 중력 영역의 방향 반환
	return GravityFields.Last()->GetGravityDirection(this);
}

