// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GravityBody.h"
#include "InputActionValue.h"
#include "Space/Game/GravityController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASpaceCharacter

ASpaceCharacter::ASpaceCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	GetCharacterMovement()->bIgnoreBaseRotation = true;
	GetCharacterMovement()->bStayBasedInAir = true; // 

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	GravityBody = CreateDefaultSubobject<UGravityBody>(TEXT("GravityBody"));

	GetCharacterMovement()->GravityScale = .0f;
}

void ASpaceCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ASpaceCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector TargetGravityDir = GravityBody->GetGravityDirection().GetSafeNormal();

	// 현재 중력 방향에서 목표 중력 방향으로 점진적으로 보간
	const float GravityChangeSpeed = 2.0f; // 중력 방향 변화 속도 (값이 클수록 빠르게 반응)
	GravityDir = FMath::VInterpTo(GravityDir, TargetGravityDir, DeltaSeconds, GravityChangeSpeed).GetSafeNormal();
	
	GetCharacterMovement()->SetGravityDirection(GravityDir);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpaceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpaceCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASpaceCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator GravityRelativeRot  = Cast<AGravityController>(Controller)->GetGravityRelativeRotation(Controller->GetControlRotation(), GravityDir);

		const FRotator YawRotation(0, GravityRelativeRot.Yaw, 0);
		const FRotator YawRollRotation(0, GravityRelativeRot.Yaw, GravityRelativeRot.Roll);

		const FRotator ForwardRotation = Cast<AGravityController>(Controller)->GetGravityWorldRotation(YawRotation, GravityDir);
		const FRotator RightRotation = Cast<AGravityController>(Controller)->GetGravityWorldRotation(YawRollRotation, GravityDir);

		// get forward vector
		FVector ForwardDirection = FRotationMatrix(ForwardRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		FVector RightDirection = FRotationMatrix(RightRotation).GetUnitAxis(EAxis::Y);

		FVector GravityNormal = GravityDir.GetSafeNormal();

		// 테스트해보니까, Forward 방향의 고각이 계속 올라감
		// Forward를 중력 방향과 수직이 되도록 보정
		const float ForwardDot = FVector::DotProduct(ForwardDirection, GravityNormal);
		ForwardDirection = ForwardDirection - ForwardDot * GravityNormal;
		ForwardDirection.Normalize();

		// Right도 마찬가지로 보정
		const float RightDot = FVector::DotProduct(RightDirection, GravityNormal);
		RightDirection -= RightDot * GravityNormal;
		RightDirection.Normalize();

		DrawDebugLine(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + ForwardDirection * 100,
		FColor::Red,
		false,              // 게임이 실행 중일 때만 그리기
		0.1f,               // 1초 동안 지속
		0,                  // 기본 깊이 우선순위
		5.0f                // 라인 두께
	);
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASpaceCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}