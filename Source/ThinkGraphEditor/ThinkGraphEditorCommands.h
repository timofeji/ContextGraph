// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "ThinkGraphEditorStyle.h"

class FThinkGraphEditorCommands : public TCommands<FThinkGraphEditorCommands>
{
public:

	FThinkGraphEditorCommands()
		: TCommands<FThinkGraphEditorCommands>(TEXT("ThinkGraphEditorCommands"), NSLOCTEXT("Contexts", "ThinkGraph", "ThinkGraph Plugin"), NAME_None, FThinkGraphEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
	TSharedPtr< FUICommandInfo > Find;
	TSharedPtr< FUICommandInfo > AddValueBindPin;
	TSharedPtr< FUICommandInfo > RemoveValueBindPin;
};