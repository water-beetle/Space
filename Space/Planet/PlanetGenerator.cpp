// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetGenerator.h"

#include "GravityFieldCenter.h"
#include "ProceduralMeshComponent.h"

// Sets default values
APlanetGenerator::APlanetGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlanetMesh"));
	RootComponent = PlanetMesh;

	OceanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OceanMesh"));
	OceanMesh->SetupAttachment(RootComponent);

	GravityField = CreateDefaultSubobject<UGravityFieldCenter>("GravityField");
	GravityField->SetupAttachment(RootComponent);

	PlanetRadius = 100;
	PlanetResolution = 100;
	WaterRadius = 100;
}

// Called when the game starts or when spawned
void APlanetGenerator::BeginPlay()
{
	Super::BeginPlay();

	GeneratePlanet(PlanetResolution, PlanetRadius);
}

void APlanetGenerator::GeneratePlanetInEditor()
{
	// 기존 메시 제거
	PlanetMesh->ClearMeshSection(0);

	// 행성 생성
	GeneratePlanet(PlanetResolution, PlanetRadius);
}

void APlanetGenerator::GeneratePlanet(int32 Resolution, float Radius)
{
	NoiseGenerator.InitNoiseParam();
	
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> Normals;

	// 각 면 생성
	GenerateFace(FVector::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(FVector::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(FVector::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);

	// Procedural Mesh 생성
	PlanetMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	if(bCreateOcean)
	{
		GenerateOcean();
	}
	
	if (PlanetMaterial)
	{
		PlanetMesh->SetMaterial(0, PlanetMaterial);
	}

	// 중력장 범위 설정
	GravityField->SetGravityFieldSize(MaxRadius + Radius * 0.4 + 100);
}

void APlanetGenerator::GenerateOcean()
{
	FString MeshPath = TEXT("/Engine/BasicShapes/Sphere.Sphere");
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath);
	
	if (SphereMesh)
	{
		OceanMesh->SetStaticMesh(SphereMesh);

		WaterRadius = FMath::Lerp(MinRadius, MaxRadius, 0.4);
		
		// 바다 반경 설정
		FVector Scale = FVector(WaterRadius / 50.0f); // 구체 기본 크기(반경 50) 기준으로 스케일 조정
		OceanMesh->SetWorldScale3D(Scale);

		// 바다 머티리얼 적용
		if(OceanMesh)
			OceanMesh->SetMaterial(0, WaterMaterial);
	}
}

void APlanetGenerator::GenerateFace(const FVector& LocalUp, int32 Resolution, float Radius, TArray<FVector>& Vertices,
                                    TArray<int32>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals)
{
	FVector AxisA = FVector(LocalUp.Y, LocalUp.Z, LocalUp.X); // Up 벡터에 직교하는 벡터
    FVector AxisB = FVector::CrossProduct(LocalUp, AxisA);    // 나머지 축 계산

	int32 VertexStartIndex = Vertices.Num(); // 현재 정점 인덱스 시작점
	
    // 정점 추가 및 캐싱
    for (int32 Y = 0; Y <= Resolution; ++Y)
    {
        for (int32 X = 0; X <= Resolution; ++X)
        {
            FVector2D GridPosition = FVector2D(X, Y);

            // 정점 위치 계산
            FVector2D Percent = GridPosition / Resolution;
            FVector PointOnCube = LocalUp + (Percent.X - 0.5f) * 2.0f * AxisA + (Percent.Y - 0.5f) * 2.0f * AxisB;
            FVector PointOnSphere = GetNormalizedPositionOnSphere(PointOnCube);

        	float Height = NoiseGenerator.GetHeight(PointOnSphere); // 노이즈 적용
        	FVector AdjustedPoint = PointOnSphere * (Height + 1) * Radius;

        	// 행성의 Min, Max Radius 업데이트
        	float CalculatedRadius = (Height + 1) * Radius;
        	MinRadius = FMath::Min(MinRadius, CalculatedRadius);
        	MaxRadius = FMath::Max(MaxRadius, CalculatedRadius);

            // 정점 추가
            Vertices.Add(AdjustedPoint);
        	
	        // UV 추가
	        FVector2D UV = CalculateUV(GridPosition, Resolution);
	        UVs.Add(UV);

        	// Normal 추가
        	FVector CalculatedNormal = CalculateNormal(PointOnSphere);
        	Normals.Add(CalculatedNormal);
        	if(bDrawDebugPoint)
        	{
        		DrawDebugLine(
				GetWorld(),
				AdjustedPoint,
				AdjustedPoint + CalculatedNormal * 5.0f, // 법선 방향
				FColor::Blue,
				false,
				10.0f,
				0,
				1.0f
			);
        	}
        	

        	// 삼각형 추가
        	if (X < Resolution && Y < Resolution)
        	{
        		int32 Current = VertexStartIndex + X + Y * (Resolution + 1);
        		int32 Next = VertexStartIndex + X + (Y + 1) * (Resolution + 1);

        		Triangles.Add(Current);
        		Triangles.Add(Next);
        		Triangles.Add(Current + 1);

        		Triangles.Add(Current + 1);
        		Triangles.Add(Next);
        		Triangles.Add(Next + 1);
        	}
        }
    }
}

FVector APlanetGenerator::GetNormalizedPositionOnSphere(const FVector& Position) const
{
	return Position.GetSafeNormal(); // 구형으로 변환
}

FVector2D APlanetGenerator::CalculateUV(const FVector2D& GridPosition, int32 Resolution)
{
	// Face내의 Resolution 한칸에 Texture 적용하는 방식으로 구현
	return GridPosition;
}

FVector APlanetGenerator::CalculateNormal(const FVector& Vertex)
{
	// 구의 Normal Vector
	FVector UnitVector = Vertex.GetSafeNormal();
	
	const float Epsilon = (NoiseGenerator.BaseNoiseScale / 100.0f) * NoiseGenerator.Frequency;

	// 노이즈 변화량 측정을 위해 축을 약간 이동
	float NoiseXPlus = NoiseGenerator.GetHeight(FVector(Vertex.X + Epsilon, Vertex.Y, Vertex.Z));
	float NoiseXMinus = NoiseGenerator.GetHeight(FVector(Vertex.X - Epsilon, Vertex.Y, Vertex.Z));
    
	float NoiseYPlus = NoiseGenerator.GetHeight(FVector(Vertex.X, Vertex.Y + Epsilon, Vertex.Z));
	float NoiseYMinus = NoiseGenerator.GetHeight(FVector(Vertex.X, Vertex.Y - Epsilon, Vertex.Z));
    
	float NoiseZPlus = NoiseGenerator.GetHeight(FVector(Vertex.X, Vertex.Y, Vertex.Z + Epsilon));
	float NoiseZMinus = NoiseGenerator.GetHeight(FVector(Vertex.X, Vertex.Y, Vertex.Z - Epsilon));

	FVector Gradient;
	Gradient.X = (NoiseXPlus - NoiseXMinus) / 2;
	Gradient.Y = (NoiseYPlus - NoiseYMinus) / 2;
	Gradient.Z = (NoiseZPlus - NoiseZMinus) / 2;

	FVector H = Gradient - (FVector::DotProduct(Gradient, UnitVector) * UnitVector);
	FVector Normal = (UnitVector - H * PlanetRadius).GetSafeNormal();
	return Normal;
}


