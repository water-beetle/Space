// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetGenerator.h"

#include "Space/Planet/Planet.h"
#include "StaticMeshAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "PhysicsEngine/BodySetup.h"
#include "Space/Planet/Sun.h"
#include "UObject/SavePackage.h"
#include "RenderingThread.h"


struct FNoiseData;

UPlanetGenerator::UPlanetGenerator()
{
	PlanetMaterialArray = TArray<UMaterialInterface*>();
}

APlanet* UPlanetGenerator::GeneratePlanet(const FString& PlanetName, int32 Resolution, float Radius, const FNoiseData& NoiseData, bool IsSun)
{
	/*
	 * 행성을 생성하는 함수
	 * 1. 행성의 Static Mesh 생성
	 * 2. 행성의 Procedural Foliage 생성(추후 구현 예정)
	 * 3. APlanet 생성 후 Static Mesh, Procedural Foliage 적용
	 */

	// 1. 행성의 Static Mesh 생성
	UStaticMesh* GeneratedPlanetMesh = GeneratePlanetMesh(PlanetName, Resolution, Radius, NoiseData);

	// 2. 행성의 Procedural Foliage 생성

	// 3. APlanet 생성 후 Static Mesh, Procedural Foliage 적용
	APlanet* GeneratedPlanet;
	if(IsSun)
	{
		GeneratedPlanet = Cast<APlanet>(GetWorld()->SpawnActor<ASun>(ASun::StaticClass(), FVector::Zero(), FRotator::ZeroRotator));
		if(SunMaterial)
		{
			GeneratedPlanet->SetPlanetMaterial(SunMaterial);
		}
	}
	else
	{
		GeneratedPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), FVector::Zero(), FRotator::ZeroRotator);
		if(PlanetMaterialArray.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, PlanetMaterialArray.Num() - 1);
			GeneratedPlanet->SetPlanetMaterial(PlanetMaterialArray[RandomIndex]);
		}
	}
	GeneratedPlanet->SetPlanetMesh(GeneratedPlanetMesh);
	
	return GeneratedPlanet;
}

UStaticMesh* UPlanetGenerator::GeneratePlanetMesh(const FString& MeshName,
                                                 int32 Resolution, float Radius, const FNoiseData& NoiseData)
{
	FString PlanetMeshFilePath = FPackageName::LongPackageNameToFilename(PlanetPackagePath + MeshName,
		FPackageName::GetAssetPackageExtension());

	// 이미 입력받은 MeshName으로 생성된 Mesh가 있다면 해당 Mesh를 반환
	if(IFileManager::Get().FileExists(*PlanetMeshFilePath))
	{
		FString AssetPath = PlanetPackagePath + MeshName + TEXT(".") + MeshName;
		
		UStaticMesh* ExistingPlanetMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(),
			nullptr, *AssetPath));
		
		if (ExistingPlanetMesh)
		{
			return ExistingPlanetMesh;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load existing mesh: %s"), *PlanetMeshFilePath);
		}
	}
	
	TArray<FVector3f> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2f> UVs;
	TArray<FVector3f> Normals;

	GenerateMeshData(Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);

	FMeshDescription MeshDescription;
	FillMeshDescription(MeshDescription, Vertices, Triangles, UVs, Normals);
	
	return CreateStaticMeshAsset(MeshName, MeshDescription);	
}

void UPlanetGenerator::GenerateMeshData(int32 Resolution, float Radius, TArray<FVector3f>& Vertices,
                                           TArray<int32>& Triangles, TArray<FVector2f>& UVs, TArray<FVector3f>& Normals, const FNoiseData& NoiseData)
{
	// 각 면 생성
	GenerateFace(FVector3f::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
	GenerateFace(-FVector3f::UpVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
	GenerateFace(FVector3f::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
	GenerateFace(-FVector3f::RightVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
	GenerateFace(FVector3f::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
	GenerateFace(-FVector3f::ForwardVector, Resolution, Radius, Vertices, Triangles, UVs, Normals, NoiseData);
}

void UPlanetGenerator::GenerateFace(const FVector3f& LocalUp, int32 Resolution, float Radius,
                                       TArray<FVector3f>& Vertices, TArray<int32>& Triangles, TArray<FVector2f>& UVs,
                                       TArray<FVector3f>& Normals, const FNoiseData& NoiseData)
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

        	float Height = NoiseGenerator.GenerateNoise(PointOnSphere, NoiseData); // 노이즈 적용
        	float scaleFactor = 0.2f;
        	FVector3f AdjustedPoint = PointOnSphere * (Radius + Radius * scaleFactor * Height);

            // 정점 추가
            Vertices.Add(AdjustedPoint);
        	
	        // UV 추가
	        FVector2f UV = CalculateUV(GridPosition, Resolution);
	        UVs.Add(UV);

        	// Normal 추가
        	//FVector3f CalculatedNormal = CalculateNormal(PointOnSphere, Radius);
        	//Normals.Add(CalculatedNormal);

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

void UPlanetGenerator::FillMeshDescription(FMeshDescription& MeshDescription, const TArray<FVector3f>& Vertices,
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

		// 노멀과 UV 설정 -> 자동 계산하도록 수정
		// if (Normals.IsValidIndex(i))
		// {
		// 	VertexNormals[InstanceID] = FVector3f(Normals[i]);
		// }
		// else
		// {
		// 	VertexNormals[InstanceID] = FVector3f(0.0f, 0.0f, 1.0f);
		// }
		
		VertexNormals[InstanceID] = FVector3f(0.0f, 0.0f, 1.0f);

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

UStaticMesh* UPlanetGenerator::CreateStaticMeshAsset(const FString& MeshName,
	const FMeshDescription& MeshDescription)
{	
	UPackage* Package = CreatePackage(*PlanetPackagePath);
	Package->FullyLoad();
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, *MeshName, RF_Public | RF_Standalone);

	// LOD 개수 설정 (LOD 0, LOD 1, LOD 2 총 3개)
	StaticMesh->SetNumSourceModels(3);

	// LOD 0 설정 (풀 디테일)
	FStaticMeshSourceModel& LOD0 = StaticMesh->GetSourceModel(0);
	LOD0.BuildSettings.bRecomputeNormals = true;
	LOD0.BuildSettings.bRecomputeTangents = true;
	LOD0.ReductionSettings.PercentTriangles = 1.0f;  // 풀 디테일 유지

	// LOD 1 설정 (중간 디테일)
	FStaticMeshSourceModel& LOD1 = StaticMesh->GetSourceModel(1);
	LOD1.BuildSettings.bRecomputeNormals = true;
	LOD1.BuildSettings.bRecomputeTangents = true;
	LOD1.ReductionSettings.PercentTriangles = 0.5f;  // 50% 삼각형 유지

	// LOD 2 설정 (저폴리곤)
	FStaticMeshSourceModel& LOD2 = StaticMesh->GetSourceModel(2);
	LOD2.BuildSettings.bRecomputeNormals = true;
	LOD2.BuildSettings.bRecomputeTangents = true;
	LOD2.ReductionSettings.PercentTriangles = 0.2f;  // 20% 삼각형 유지

	StaticMesh->GetStaticMaterials().Add(FStaticMaterial());  // 기본 재질 추가

	// MeshDescription 적용
	StaticMesh->CreateMeshDescription(0, MeshDescription);
	StaticMesh->SetLightingGuid(FGuid::NewGuid());  

	// Lightmap Resolution 설정
	StaticMesh->LightMapResolution = 512; 

	// 충돌 및 거리 필드 설정
	StaticMesh->bGenerateMeshDistanceField = false;
	StaticMesh->bHasNavigationData = true;
	StaticMesh->CreateBodySetup();
	UBodySetup* BodySetup = StaticMesh->GetBodySetup();
	if (BodySetup)
	{
		BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		BodySetup->CreatePhysicsMeshes();
	}
	
	StaticMesh->CommitMeshDescription(0);
	
	StaticMesh->Build(true);
	FlushRenderingCommands();
	StaticMesh->PostEditChange();

	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(StaticMesh);
	
	const FString FullPath = FPackageName::LongPackageNameToFilename(PlanetPackagePath + MeshName, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	
	if (!UPackage::SavePackage(Package, StaticMesh, *FullPath, SaveArgs))
	{
	    UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *FullPath);
	}

	return StaticMesh;
}

FVector3f UPlanetGenerator::GetNormalizedPositionOnSphere(const FVector3f& Position) const
{
	return Position.GetSafeNormal(); // 구형으로 변환
}

FVector2f UPlanetGenerator::CalculateUV(const FVector2f& GridPosition, int32 Resolution)
{
	// Face내의 Resolution 한칸에 Texture 적용하는 방식으로 구현
	return GridPosition;
}

// FVector3f UPlanetGenerator::CalculateNormal(const FVector3f& Vertex, float PlanetRadius)
// {
// 	// 구의 Normal Vector
// 	FVector3f UnitVector = Vertex.GetSafeNormal();
// 	
// 	const float Epsilon = (NoiseGenerator.NoiseData.BaseNoiseScale / 100.0f) * NoiseGenerator.NoiseData.Frequency;
//
// 	// 노이즈 변화량 측정을 위해 축을 약간 이동
// 	float NoiseXPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X + Epsilon, Vertex.Y, Vertex.Z));
// 	float NoiseXMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X - Epsilon, Vertex.Y, Vertex.Z));
//     
// 	float NoiseYPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y + Epsilon, Vertex.Z));
// 	float NoiseYMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y - Epsilon, Vertex.Z));
//     
// 	float NoiseZPlus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y, Vertex.Z + Epsilon));
// 	float NoiseZMinus = NoiseGenerator.GetHeight(FVector3f(Vertex.X, Vertex.Y, Vertex.Z - Epsilon));
//
// 	FVector3f Gradient;
// 	Gradient.X = (NoiseXPlus - NoiseXMinus) / 2;
// 	Gradient.Y = (NoiseYPlus - NoiseYMinus) / 2;
// 	Gradient.Z = (NoiseZPlus - NoiseZMinus) / 2;
//
// 	FVector3f H = Gradient - (FVector3f::DotProduct(Gradient, UnitVector) * UnitVector);
// 	FVector3f Normal = (UnitVector - H * PlanetRadius).GetSafeNormal();
// 	return Normal;
// }
