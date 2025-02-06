// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "PlanetNoiseData.h"
#include "Sun.h"


// Sets default values
ASolarSystem::ASolarSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MinOrbitRadius = 10000;
	MaxOrbitRadius = 100000;

	PlanetMaterialArray = TArray<UMaterialInterface*>();
	
	SunPosition = FVector::ZeroVector;
	SunRadius = 5000;
	SunResolution = 100;
	SunNoiseData.SetZero();
	SunOrbitData.SetZero();
	
	PlanetNums = 10;
	PlanetMinRadius = 1000;
	PlanetMaxRadius = 5000;
	PlanetResolution = 100;
	bUseNewPlanetMesh = false;
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();
	
	SetRandomPlanetData();
	GeneratePlanetMesh();
	GenerateSun();
	GeneratePlanets();
	
	// 가끔씩 함수가 실행이 안됨 -> 다른 Actor들이 초기화 되기 전에 실행되어 그런듯, 다음 프레임에서 실행되도록 수정
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASolarSystem::PlaceMainCharacter, .3f, false);
	
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASolarSystem::SetRandomPlanetData()
{
	for (int i = 0; i < PlanetNums; ++i)
	{
		FPlanetData PlanetData;
		PlanetData.Radius = FMath::RandRange(PlanetMinRadius, PlanetMaxRadius);
		PlanetData.OrbitData.SetRandom(MinOrbitRadius, MaxOrbitRadius);
		PlanetDataArray.Add(PlanetData);
	}
}

void ASolarSystem::GeneratePlanetMesh()
{
	FString SunPath = FPackageName::LongPackageNameToFilename(PlanetSaveDirectory + "Sun",
		FPackageName::GetAssetPackageExtension());

	if(!IFileManager::Get().FileExists(*SunPath))
	{
		MeshGenerator.GeneratePlanetMesh(PlanetSaveDirectory, "Sun", SunResolution, SunRadius, SunNoiseData);
	}
	
	for (int i = 0; i < PlanetNums; ++i)
	{
		FString PlanetPath = FPackageName::LongPackageNameToFilename(PlanetSaveDirectory + PlanetMeshName +
			FString::FromInt(i), FPackageName::GetAssetPackageExtension());

		if(bUseNewPlanetMesh || !IFileManager::Get().FileExists(*PlanetPath))
		{
			MeshGenerator.GeneratePlanetMesh(PlanetSaveDirectory, PlanetMeshName + FString::FromInt(i),
				PlanetResolution, PlanetDataArray[i].Radius, NoiseDataAsset->NoiseSettings);
		}
	}
}

void ASolarSystem::GenerateSun()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	Sun = GetWorld()->SpawnActor<ASun>(ASun::StaticClass(), SunPosition, FRotator::ZeroRotator, SpawnParams);
	Sun->InitPlanet(SunOrbitData, SunPosition, SunRadius);
	Sun->SetDiretionalLight(DirectionalSunLight);
	Sun->SetActorLocation(SunPosition);
	
	FString SunPath = PlanetSaveDirectory + TEXT("Sun");
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *SunPath);
	Sun->SetPlanetMeshAndMaterial(Mesh, SunMaterial);
}

void ASolarSystem::GeneratePlanets()
{
	for (int i = 0; i < PlanetNums; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
	
		APlanet* Planet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParams);
		Planet->InitPlanet(PlanetDataArray[i].OrbitData, SunPosition, PlanetDataArray[i].Radius);
	
		FString PlanetPath = PlanetSaveDirectory + PlanetMeshName + FString::FromInt(i);;
		UStaticMesh* PlanetMesh = LoadObject<UStaticMesh>(nullptr, *PlanetPath);
		UMaterialInterface* PlanetMaterial = nullptr;
		if(PlanetMaterialArray.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, PlanetMaterialArray.Num() - 1);
			PlanetMaterial = PlanetMaterialArray[RandomIndex];
		}
		
		Planet->SetPlanetMeshAndMaterial(PlanetMesh, PlanetMaterial);

		PlanetDataArray[i].Planet = Planet;
	}
}

void ASolarSystem::PlaceMainCharacter()
{
	// 무작위 번째의 행성에서 플레이어를 생성
	// 행성 공전, 자전중에 플레이어를 위치시키니까 캐릭터가 행성위에 제대로 서있지 못함
	// 캐릭터를 행성에 먼저 배치하고, 행성 자전, 공전 코드 실행
	
	if(APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PlayerPawn = PC->GetPawn();
	}

	if(PlayerPawn)
	{
		int randInd = FMath::RandRange(0, PlanetNums-1);
		if(PlanetNums > 0)
		{
			FVector EndPoint = PlanetDataArray[randInd].Planet->GetActorLocation();;
			FVector StartPoint = EndPoint + FVector::UpVector * PlanetDataArray[randInd].Radius * 2;

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;

			if(GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, QueryParams))
			{
				FVector SpawnLocation = HitResult.Location + (HitResult.Location - EndPoint).GetSafeNormal() * 100.f; // 약간 위에 스폰
				FRotator SpawnRotation = FRotationMatrix::MakeFromZ((HitResult.Location - EndPoint)).Rotator();
				
				PlayerPawn->SetActorLocation(SpawnLocation);
				PlayerPawn->SetActorRotation(SpawnRotation);
				PlanetDataArray[randInd].Planet->AddCharacterToGravityField();
				
			}
		}
	}

	// 타이머 없이 공전, 자전하니 캐릭터가 착륙할 시간이 부족 
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASolarSystem::SetMovePlanet, .5f, false);
}

void ASolarSystem::SetMovePlanet()
{
	for(const auto& PlanetData : PlanetDataArray)
	{
		PlanetData.Planet->SetMove(true);
	}
}

