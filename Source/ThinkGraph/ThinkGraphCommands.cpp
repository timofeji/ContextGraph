// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphCommands.h"

#define LOCTEXT_NAMESPACE "FThinkGraphModule"

void FThinkGraphCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ThinkGraph", "Bring up ThinkGraph window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
