#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"  // USphereComponent 헤더 포함
#include "GravityField.generated.h"

class UBoxComponent;
class UGravityBody;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACE_API UGravityField : public USphereComponent
{
	GENERATED_BODY()

public:
	// 기본 생성자
	UGravityField();

protected:
	virtual void BeginPlay() override;

public:
	// 중력 방향 계산 함수
	virtual FVector GetGravityDirection(UGravityBody* GravityBody) const PURE_VIRTUAL(UGravityField::GetGravityDirection, return FVector::ZeroVector;);

	// 중력 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	int Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float GravityScale;

	void SetGravityFieldSize(float Radius);
	void AddCharacterToGravityField();

private:
	// 중력 필드 충돌 처리
	UFUNCTION()
	void OnEnterGravityArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExitGravityArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};