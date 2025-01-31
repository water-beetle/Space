// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetOrbit.h"

#include "Space/Data/PlanetData.h"


// Sets default values for this component's properties
UPlanetOrbit::UPlanetOrbit()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/Materials/Space/M_Orbit.M_Orbit"));
	if (MaterialFinder.Succeeded())
	{
		OrbitMaterial = MaterialFinder.Object;
	}
	
	OrbitRadiusX = 10000;
	OrbitRadiusY = 5000;
	OrbitIncline = FRotator(10, 10, 10);
	OrbitSpeed = 10;
	RotationSpeed = 10;
	CurrentOrbitTheta = 0;
	CurrentRotationTheta = 0;
	OrbitCenter = FVector::ZeroVector;
	SegmentCount = 100;
	
	Thickness = 10000;
}


// Called when the game starts
void UPlanetOrbit::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPlanetOrbit::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlanetRevolve(DeltaTime);
	PlanetRotation(DeltaTime);
}

void UPlanetOrbit::InitOrbit(const FOrbitData& OrbitData, const FVector _OrbitCenter)
{
	OrbitRadiusX = OrbitData.OrbitRadiusX;
	OrbitRadiusY = OrbitData.OrbitRadiusY;
	OrbitSpeed = OrbitData.OrbitSpeed;
	OrbitIncline = OrbitData.OrbitIncline;
	RotationSpeed = OrbitData.RotationSpeed;
	SegmentCount = OrbitData.SegmentCount;
	Thickness = OrbitData.Thickness;
	OrbitCenter = _OrbitCenter;

	// 공전 각속도 설정
	float EllipseCircumference = CalculateEllipseCircumference(OrbitRadiusX, OrbitRadiusY);
	float RotateSpeed = EllipseCircumference / OrbitSpeed;
	AngularSpeed = 360.0f / RotateSpeed;
	
	
	// 랜덤 각도 생성 (0 ~ 360°)
	CurrentOrbitTheta = FMath::FRandRange(0.0f, 360.0f);

	// 타원 궤도 상의 초기 위치 계산
	float Radians = FMath::DegreesToRadians(CurrentOrbitTheta);
	FVector InitialPosition = FVector(
		OrbitRadiusX * FMath::Cos(Radians),
		OrbitRadiusY * FMath::Sin(Radians),
		0.0f
	);

	// 궤도 기울기 적용
	InitialPosition = OrbitIncline.RotateVector(InitialPosition);

	// 중심 위치 기준으로 최종 위치 보정
	InitialPosition += OrbitCenter;

	// 위치 설정
	GetOwner()->SetActorLocation(InitialPosition);
}

void UPlanetOrbit::SetOrbitVisualization(UProceduralMeshComponent* OrbitMesh)
{
	GenerateEllipseMesh(OrbitMesh);

	// 타원 궤도를 Static Mesh 스케일로 조정
	OrbitMesh->SetWorldLocation(OrbitCenter);
}

void UPlanetOrbit::GenerateEllipseMesh(UProceduralMeshComponent* OrbitMesh)
{
	if (SegmentCount < 3) SegmentCount = 3; // 최소 3

    // 외부 반지름
    float OuterMajorRadius = OrbitRadiusX;
    float OuterMinorRadius = OrbitRadiusY;

    // 내부 반지름 = 바깥 - 두께
    float InnerMajorRadius = FMath::Max(0.f, OrbitRadiusX - Thickness);
    float InnerMinorRadius = FMath::Max(0.f, OrbitRadiusY - Thickness);

    // 버텍스, 인덱스 배열
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FLinearColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    // 타원 링이므로, 외곽/내곽 각각 SegmentCount개씩 (총 2*SegmentCount)
    Vertices.SetNum(SegmentCount * 2);
    Normals.SetNum(SegmentCount * 2);
    UVs.SetNum(SegmentCount * 2);
    VertexColors.SetNum(SegmentCount * 2);
    Tangents.SetNum(SegmentCount * 2);

    for (int i = 0; i < SegmentCount; i++)
    {
    	float Theta = (360.0f / SegmentCount) * i;
    	
    	FVector OuterPosition = CalculateOrbitPointsByTheta(Theta, OuterMajorRadius, OuterMinorRadius);
		FVector InnerPosition = CalculateOrbitPointsByTheta(Theta, InnerMajorRadius, InnerMinorRadius);

        // Vertices (Z=0 기준)
        Vertices[i] = OuterPosition;                // 바깥
        Vertices[i + SegmentCount] = InnerPosition;     // 안쪽

        // 모든 노멀은 +Z 방향(위쪽)을 가리키게
        Normals[i] = FVector::UpVector;
        Normals[i + SegmentCount] = FVector::UpVector;

        // UV는 간단히 i/SegmentCount 로 줄 수도 있음
        float UCoord = (float)i / (float)SegmentCount;
        UVs[i] = FVector2D(UCoord, 0.f);          // 바깥
        UVs[i + SegmentCount] = FVector2D(UCoord, 1.f); // 안쪽

        // 색상 (원하면 흰색 통일)
        VertexColors[i] = FLinearColor::White;
        VertexColors[i + SegmentCount] = FLinearColor::White;

        // 탄젠트는 대략 X축 방향으로 설정(임의)
        Tangents[i] = FProcMeshTangent(1.f, 0.f, 0.f);
        Tangents[i + SegmentCount] = FProcMeshTangent(1.f, 0.f, 0.f);
    }

    // Triangles: 세그먼트별로 두 개의 삼각형
    // (i, i+1, i+SegmentCount), (i+1, i+1+SegmentCount, i+SegmentCount)
    // 단, i+1이 SegmentCount를 넘어가면 0으로 래핑
    for (int i = 0; i < SegmentCount; i++)
    {
        int iNext = (i + 1) % SegmentCount;

        // 첫 삼각형
        Triangles.Add(i);
        Triangles.Add(iNext);
        Triangles.Add(i + SegmentCount);

        // 두 번째 삼각형
        Triangles.Add(iNext);
        Triangles.Add(iNext + SegmentCount);
        Triangles.Add(i + SegmentCount);
    }

    // 이전 섹션 지우고 새 섹션 만들기 (인덱스 0번 섹션)
    OrbitMesh->ClearAllMeshSections();
    OrbitMesh->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        Normals,
        UVs,
        VertexColors,
        Tangents,
        false  // 생성된 메시를 렌더링 할지 여부 (true면 충돌용 물리도 생성)
    );
	
    OrbitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (OrbitMaterial)
	{
		OrbitMesh->SetMaterial(0, OrbitMaterial);
	}
}

void UPlanetOrbit::PlanetRevolve(float DeltaTime)
{
	CurrentOrbitTheta += AngularSpeed * DeltaTime;
	if (CurrentOrbitTheta >= 360.0f) CurrentOrbitTheta -= 360.0f;

	FVector OrbitPoint = CalculateOrbitPointsByTheta(CurrentOrbitTheta, OrbitRadiusX, OrbitRadiusY) + OrbitCenter;
	GetOwner()->SetActorLocation(OrbitPoint);	
}

void UPlanetOrbit::PlanetRotation(float DeltaTime)
{
	// 자전 각도 업데이트
	CurrentRotationTheta += RotationSpeed * DeltaTime;
	if (CurrentRotationTheta >= 360.0f) CurrentRotationTheta -= 360.0f;

	// 자전 적용
	FRotator NewRotation = GetOwner()->GetActorRotation();
	NewRotation.Yaw = CurrentRotationTheta;
	GetOwner()->SetActorRotation(NewRotation);
}

FVector UPlanetOrbit::CalculateOrbitPointsByTheta(float Theta, float RadiusX, float RadiusY) const
{
	// 타원 궤도 상의 새로운 위치 계산
	float Radians = FMath::DegreesToRadians(Theta);
	FVector NewPosition = FVector(
		RadiusX * FMath::Cos(Radians),
		RadiusY * FMath::Sin(Radians),
		0.0f
	);

	// 궤도 기울기 적용
	return OrbitIncline.RotateVector(NewPosition);
}

float UPlanetOrbit::CalculateEllipseCircumference(float RadiusX, float RadiusY) const
{
	// 타원 둘레 근사
	return PI * (3 * (RadiusX + RadiusY) - FMath::Sqrt((3 * RadiusX + RadiusY) * (RadiusX + 3 * RadiusY)));
}

