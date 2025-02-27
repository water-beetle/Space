// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "Sun.h"
#include "ProceduralGeneration/PlanetNoiseData.h"
#include "Space/Characters/SpaceCharacter.h"


// Sets default values
ASolarSystem::ASolarSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlanetGenerator = CreateDefaultSubobject<UPlanetGenerator>(TEXT("PlanetGenerator"));
	static ConstructorHelpers::FClassFinder<ACharacter> RobotBP(
			TEXT("/Game/Blueprints/BP_FunnyRobot.BP_FunnyRobot_C")
		);
	if (RobotBP.Succeeded())
	{
		FunnyRobotBPClass = RobotBP.Class;
	}

	
	MinOrbitRadius = 10000;
	MaxOrbitRadius = 100000;
	
	SunPosition = FVector::ZeroVector;
	SunRadius = 5000;
	SunResolution = 100;
	SunNoiseData.SetZero();
	SunOrbitData.SetZero();
	
	PlanetNums = 10;
	PlanetMinRadius = 1000;
	PlanetMaxRadius = 5000;
	PlanetResolution = 100;
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateSolarSystem();

	// 문제
	// Static Mesh를 생성할때 여러 쓰레드로 처리하면서 언제 최종적으로 완료되는지를 알 수가 없음
	// 그냥 로딩화면 몇분 띄워놔야 될듯...
	
	PlaceMainCharacter();
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASolarSystem::GenerateSolarSystem()
{
	/* 태양계를 생성하는 만드는 함수
	 * 1. 태양 생성
	 * 2. 행성 생성
	 */

	GenerateSun();
	GeneratePlanets();
}

void ASolarSystem::GenerateSun()
{
	/* 태양을 생성하는 함수
	 * 1. 태양 Actor 생성
	 * 2. 태양 기본 설정
	 * 4. 태양 설정한 위치에 배치
	 * 5. 태양 Mesh 적용
	 */
	FString SunName = TEXT("Sun");
	Sun = Cast<ASun>(PlanetGenerator->GeneratePlanet(SunName, SunResolution, SunRadius, SunNoiseData, true));
	Sun->InitSun(SunOrbitData, SunRadius, DirectionalSunLight);
	Sun->SetActorLocation(SunPosition);
}

void ASolarSystem::GeneratePlanets()
{
	/* 행성들을 생성하는 함수
	 * 1. 행성 생성을 위한 파라미터 랜덤 생성 : 반지름 / 자전,공전
	 * 2. 행성 Actor 생성
	 * 3. 행성 기본 설정
	 * 4.
	 */
	
	for (int i = 0; i < PlanetNums; ++i)
	{
		FString PlanetName = FString::Printf(TEXT("Planet_%d"), i);
		float PlanetRadius = FMath::RandRange(PlanetMinRadius, PlanetMaxRadius);
		FOrbitData OrbitData;
		OrbitData.SetRandom(MaxOrbitRadius, MinOrbitRadius);
		OrbitData.OrbitCenter = SunPosition;
		
		APlanet* Planet = PlanetGenerator->GeneratePlanet(PlanetName, PlanetResolution, PlanetRadius,
			NoiseDataAsset->NoiseSettings, false);
		Planet->InitPlanet(OrbitData, PlanetRadius);
	
		FString PlanetPath = PlanetSaveDirectory + PlanetMeshName + FString::FromInt(i);;
		UStaticMesh* PlanetMesh = LoadObject<UStaticMesh>(nullptr, *PlanetPath);
		UMaterialInterface* PlanetMaterial = nullptr;

		PlanetArray.Add(Planet);
	}
}

void ASolarSystem::PlaceMainCharacter()
{

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASolarSystem::TryPlaceCharacter, 0.5f, false);
}

void ASolarSystem::TryPlaceCharacter()
{
	// 무작위 번째의 행성에서 플레이어를 생성
    	// 행성 공전, 자전중에 플레이어를 위치시키니까 캐릭터가 행성위에 제대로 서있지 못함
    	// 캐릭터를 행성에 먼저 배치하고, 행성 자전, 공전 코드 실행
    	
    
    	int randInd = FMath::RandRange(0, PlanetNums-1);
    	if(PlanetNums > 0)
    	{
    		FVector EndPoint = PlanetArray[randInd]->GetActorLocation();
    		FVector StartPoint = EndPoint + FVector::UpVector * PlanetArray[randInd]->GetRadius() * 2;
    
    		FHitResult HitResult;
    		FCollisionQueryParams QueryParams;

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, QueryParams);
    		
    		if(bHit)
    		{
    			FVector SpawnLocation = HitResult.Location + (HitResult.Location - EndPoint).GetSafeNormal() * 100.f; // 약간 위에 스폰
    			FRotator SpawnRotation = FRotationMatrix::MakeFromZ((HitResult.Location - EndPoint)).Rotator();
    			
    			FActorSpawnParameters SpawnParams;
    			SpawnParams.Owner = this;        // SolarSystem 액터가 오너일 수도 있고, 필요없으면 생략 가능
    			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    			// 실제 Pawn 클래스를 지정해야 함 (예: AMyPlayerCharacter)
    			if (FunnyRobotBPClass != nullptr)
    			{
    				SpaceCharacterPlayer = GetWorld()->SpawnActor<ASpaceCharacter>(FunnyRobotBPClass, SpawnLocation, SpawnRotation);
    			}
    			if (SpaceCharacterPlayer)
    			{
    				// PlayerController가 새 Pawn을 소유하도록
    				if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    				{
    					PC->Possess(SpaceCharacterPlayer);
    				}
    
    				// 스폰된 Pawn을 행성의 중력장에 등록
    				PlanetArray[randInd]->AddCharacterToGravityField();
    				
    				// 타이머 없이 공전, 자전하니 캐릭터가 착륙할 시간이 부족 
    				FTimerHandle TimerHandle;
    				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASolarSystem::SetMovePlanet, .5f, false);
    			}
    		}
		    else
		    {
		    	FTimerHandle RetryHandle;
		    	GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &ASolarSystem::TryPlaceCharacter, 0.5f, false);
		    }
    	}
}

void ASolarSystem::SetMovePlanet()
{
	for(const auto& Planet : PlanetArray)
	{
		Planet->SetMove(true);
	}
}

