// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphEditorCommands.h"

#define LOCTEXT_NAMESPACE "FThinkGraphEditorCommands"

void FThinkGraphEditorCommands::RegisterCommands()
{
	UI_COMMAND(Find, "Find", "Find within graph", EUserInterfaceActionType::Button,
	           FInputChord(EModifierKey::Control, EKeys::F));
	UI_COMMAND(OpenPluginWindow, "Dialogue", "Bring up Dialogue window", EUserInterfaceActionType::Button,
	           FInputChord());
	UI_COMMAND(AddValueBindPin, "Add Value Bind", "Add Value Bind to the list", EUserInterfaceActionType::Button,
	           FInputChord());
	UI_COMMAND(RemoveValueBindPin, "Remove Value Bind", "Remove Value Bind from the list",
	           EUserInterfaceActionType::Button,
	           FInputChord());
}

#undef LOCTEXT_NAMESPACE
