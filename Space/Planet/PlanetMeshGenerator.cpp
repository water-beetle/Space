// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetMeshGenerator.h"
#include "StaticMeshAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PhysicsEngine/BodySetup.h"
#include "UObject/SavePackage.h"


struct FNoiseData;

PlanetMeshGenerator::PlanetMeshGenerator()
{
}

PlanetMeshGenerator::~PlanetMeshGenerator()
{
}

UStaticMesh* PlanetMeshGenerator::GeneratePlanetMesh(const FString& PackagePath, const FString& MeshName,
	int32 Resolution, float Radius, const FNoiseData& NoiseData)
{
	NoiseGenerator.InitNoiseParam(NoiseData);
	
	TArray<FVector3f> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2f> UVs;
	TArray<FVector3f> Normals;

	GenerateMeshData(Resolution, Radius, Vertices, Triangles, UVs, Normals);

	FMeshDescription MeshDescription;
	FillMeshDescription(MeshDescription, Vertices, Triangles, UVs, Normals);
	
	return CreateStaticMeshAsset(PackagePath, MeshName, MeshDescription);	
}

void PlanetMeshGenerator::GenerateMeshData(int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
                                           TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals)
{
	// 각 면 생성
	GenerateFace(FVector3f::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector3f::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(FVector3f::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector3f::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(FVector3f::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
	GenerateFace(-FVector3f::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals);
}

void PlanetMeshGenerator::GenerateFace(const FVector3f& LocalUp, int32 Resolution, float Radius,
                                       TArray<FVector3f>& Vertices, TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals)
{
	FVector3f AxisA = FVector3f(LocalUp.Y, LocalUp.Z, LocalUp.X); // Up 벡터에 직교하는 벡터
    FVector3f AxisB = FVector3f::CrossProduct(LocalUp, AxisA);    // 나머지 축 계산

	int32 VertexStartIndex = Vertices.Num(); // 현재 정점 인덱스 시작점
	
    // 정점 추가 및 캐싱
    for (int32 Y = 0; Y <= Resolution; ++Y)
    {
        for (int32 X = 0; X <= Resolution; ++X)
        {
            FVector2f GridPosition = FVector2f(X, Y);

            // 정점 위치 계산
            FVector2f Percent = GridPosition / Resolution;
            FVector3f PointOnCube = LocalUp + (Percent.X - 0.5f) * 2.0f * AxisA + (Percent.Y - 0.5f) * 2.0f * AxisB;
            FVector3f PointOnSphere = GetNormalizedPositionOnSphere(PointOnCube);

        	float Height = NoiseGenerator.GetHeight(PointOnSphere); // 노이즈 적용
        	FVector3f AdjustedPoint = PointOnSphere * (Height + 1) * Radius;

            // 정점 추가
            Vertices.Add(AdjustedPoint);
        	
	        // UV 추가
	        FVector2f UV = CalculateUV(GridPosition, Resolution);
	        UVs.Add(UV);

        	// Normal 추가
        	FVector3f CalculatedNormal = CalculateNormal(PointOnSphere, Radius);
        	Normals.Add(CalculatedNormal);

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

void PlanetMeshGenerator::FillMeshDescription(FMeshDescription& MeshDescription, const TArray<FVector3f>& Vertices,
	const TArray<int32>& Triangles, const TArray<FVector2f>& UVs, const TArray<FVector3f>& Normals)
{
	// 1. MeshDescription 속성 설정
    FStaticMeshAttributes Attributes(MeshDescription);
    Attributes.Register();

	TVertexAttributesRef<FVector3f> VertexPositions = Attributes.GetVertexPositions();
	TVertexInstanceAttributesRef<FVector3f> VertexNormals = Attributes.GetVertexInstanceNormals();
	TVertexInstanceAttributesRef<FVector2f> VertexUVs = Attributes.GetVertexInstanceUVs();

	// 정점(Vertex) 및 인스턴스(Vertex Instance) 생성
	TMap<int32, FVertexInstanceID> VertexInstanceMap;
	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		// 정점 생성 및 위치 설정
		FVertexID VertexID = MeshDescription.CreateVertex();
		VertexPositions[VertexID] = FVector3f(Vertices[i]);

		// 정점 인스턴스 생성
		FVertexInstanceID InstanceID = MeshDescription.CreateVertexInstance(VertexID);

		// 노멀과 UV 설정
		if (Normals.IsValidIndex(i))
		{
			VertexNormals[InstanceID] = FVector3f(Normals[i]);
		}
		else
		{
			VertexNormals[InstanceID] = FVector3f(0.0f, 0.0f, 1.0f);
		}

		if (UVs.IsValidIndex(i))
		{
			VertexUVs[InstanceID] = UVs[i];
		}
		else
		{
			VertexUVs[InstanceID] = FVector2f(0.0f, 0.0f);  // 기본 UV 값
		}

		// 인스턴스 매핑
		VertexInstanceMap.Add(i, InstanceID);
	}

	// 폴리곤 그룹 생성
	FPolygonGroupID PolygonGroup = MeshDescription.CreatePolygonGroup();

	// 삼각형 생성
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		if (!VertexInstanceMap.Contains(Triangles[i]) ||
			!VertexInstanceMap.Contains(Triangles[i + 1]) ||
			!VertexInstanceMap.Contains(Triangles[i + 2]))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid triangle indices at %d, %d, %d"), Triangles[i], Triangles[i + 1], Triangles[i + 2]);
			continue;
		}

		// 삼각형을 구성하는 VertexInstanceIDs 가져오기
		TArray<FVertexInstanceID> InstanceIDs = {
			VertexInstanceMap[Triangles[i]],
			VertexInstanceMap[Triangles[i + 1]],
			VertexInstanceMap[Triangles[i + 2]]
		};

		// 폴리곤 생성
		MeshDescription.CreatePolygon(PolygonGroup, InstanceIDs);
	}

	UE_LOG(LogTemp, Log, TEXT("Vertices: %d, Triangles: %d"), Vertices.Num(), Triangles.Num() / 3);
}

UStaticMesh* PlanetMeshGenerator::CreateStaticMeshAsset(const FString& PackagePath, const FString& MeshName,
	const FMeshDescription& MeshDescription)
{	
	UPackage* Package = CreatePackage(*PackagePath);
	Package->FullyLoad();
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, *MeshName, RF_Public | RF_Standalone);

	FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
	SourceModel.BuildSettings.bRecomputeNormals = false;
	SourceModel.BuildSettings.bRecomputeTangents = false;

	StaticMesh->GetStaticMaterials().Add(FStaticMaterial());  // 기본 재질 추가

	// 2. MeshDescription 적용 및 커밋
	StaticMesh->CreateMeshDescription(0, MeshDescription);
	StaticMesh->SetNumSourceModels(1);
	StaticMesh->SetLightingGuid(FGuid::NewGuid());  // 조명에 필요한 GUID 생성
	
	// **충돌 관련 설정**
	StaticMesh->bGenerateMeshDistanceField = true;                // 거리 필드 생성
	StaticMesh->bHasNavigationData = true;                        // 내비게이션 관련 데이터 포함
	StaticMesh->CreateBodySetup();
	UBodySetup* BodySetup = StaticMesh->GetBodySetup();
	if (BodySetup)
	{
		BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;  // 복잡 충돌 사용
		BodySetup->CreatePhysicsMeshes();                        // 물리 충돌 데이터 생성
	}
	
	StaticMesh->CommitMeshDescription(0);
	
	StaticMesh->Build(false);
	StaticMesh->PostEditChange();

	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(StaticMesh);
	
	const FString FullPath = FPackageName::LongPackageNameToFilename(PackagePath + MeshName, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	
	if (!UPackage::SavePackage(Package, StaticMesh, *FullPath, SaveArgs))
	{
	    UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *FullPath);
	}

	return StaticMesh;
}

FVector3f PlanetMeshGenerator::GetNormalizedPositionOnSphere(const FVector3f& Position) const
{
	return Position.GetSafeNormal(); // 구형으로 변환
}

FVector2f PlanetMeshGenerator::CalculateUV(const FVector2f& GridPosition, int32 Resolution)
{
	// Face내의 Resolution 한칸에 Texture 적용하는 방식으로 구현
	return GridPosition;
}

FVector3f PlanetMeshGenerator::CalculateNormal(const FVector3f& Vertex, float PlanetRadius)
{
	// 구의 Normal Vector
	FVector3f UnitVector = Vertex.GetSafeNormal();
	
	const float Epsilon = (NoiseGenerator.NoiseData.BaseNoiseScale / 100.0f) * NoiseGenerator.NoiseData.Frequency;

	// 노이즈 변화량 측정을 위해 축을 약간 이동
	float NoiseXPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X + Epsilon, Vertex.Y, Vertex.Z));
	float NoiseXMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X - Epsilon, Vertex.Y, Vertex.Z));
    
	float NoiseYPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y + Epsilon, Vertex.Z));
	float NoiseYMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y - Epsilon, Vertex.Z));
    
	float NoiseZPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y, Vertex.Z + Epsilon));
	float NoiseZMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y, Vertex.Z - Epsilon));

	FVector3f Gradient;
	Gradient.X = (NoiseXPlus - NoiseXMinus) / 2;
	Gradient.Y = (NoiseYPlus - NoiseYMinus) / 2;
	Gradient.Z = (NoiseZPlus - NoiseZMinus) / 2;

	FVector3f H = Gradient - (FVector3f::DotProduct(Gradient, UnitVector) * UnitVector);
	FVector3f Normal = (UnitVector - H * PlanetRadius).GetSafeNormal();
	return Normal;
}
