// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Space/Planet/Physics/PlanetOrbit.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

class UPlanetOrbit;
class UGravityFieldCenter;

UCLASS()
class SPACE_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties	
	APlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	UStaticMeshComponent* PlanetMesh;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	UGravityFieldCenter* GravityField;
	UPROPERTY(EditAnywhere, Category = "Planet")
	float PlanetRadius;

	void SetPlanetMesh(UStaticMesh* StaticMesh);
	void SetPlanetMaterial(UMaterialInterface* PlanetMaterial);
	
	virtual void InitPlanet(const FOrbitData& OrbitData, float Radius);

	void AddCharacterToGravityField();
	
	// 행성의 공전, 자전을 설정하는 함수
	FORCEINLINE void SetMove(bool bMove) const {PlanetOrbitComponent->SetMove((bMove));}
	FORCEINLINE float GetRadius() const {return PlanetRadius;}

private:
	UPROPERTY(EditAnywhere, Category = "Planet")
	UPlanetOrbit* PlanetOrbitComponent;
};
