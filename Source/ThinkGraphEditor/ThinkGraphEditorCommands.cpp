// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphEditorCommands.h"

#define LOCTEXT_NAMESPACE "FDialogueModule"

void FThinkGraphEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Dialogue", "Bring up Dialogue window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
