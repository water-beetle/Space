// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravityField.h"
#include "GravityFieldCenter.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACE_API UGravityFieldCenter : public UGravityField
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UGravityFieldCenter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual FVector GetGravityDirection(UGravityBody* GravityBody) const override;
};
