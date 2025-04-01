// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoidsGameMode.h"
#include "BoidsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABoidsGameMode::ABoidsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
