// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceGameMode.h"

#include "GravityController.h"
#include "Space/Characters/SpaceCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpaceGameMode::ASpaceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_FunnyRobot"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		//DefaultPawnClass = PlayerPawnBPClass.Class;
		DefaultPawnClass = nullptr;
	}

	PlayerControllerClass = AGravityController::StaticClass();
}