// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityFieldCenter.h"

#include "Space/Characters/GravityBody.h"


// Sets default values
UGravityFieldCenter::UGravityFieldCenter()
{

}

// Called when the game starts or when spawned
void UGravityFieldCenter::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector UGravityFieldCenter::GetGravityDirection(UGravityBody* GravityBody) const
{
	return (GetComponentLocation() - GravityBody->GetOwner()->GetActorLocation()).GetSafeNormal();
}

