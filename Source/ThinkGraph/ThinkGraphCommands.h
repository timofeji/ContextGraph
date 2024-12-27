// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "ThinkGraphStyle.h"

class FThinkGraphCommands : public TCommands<FThinkGraphCommands>
{
public:

	FThinkGraphCommands()
		: TCommands<FThinkGraphCommands>(TEXT("ThinkGraph"), NSLOCTEXT("Contexts", "ThinkGraph", "ThinkGraph Plugin"), NAME_None, FThinkGraphStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};