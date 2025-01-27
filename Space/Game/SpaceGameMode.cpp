// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceGameMode.h"
#include "Space/Characters/SpaceCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpaceGameMode::ASpaceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
