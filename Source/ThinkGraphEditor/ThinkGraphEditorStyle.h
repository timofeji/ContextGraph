// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"

/**  */
class FThinkGraphEditorStyle
{
public:
	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

	FORCEINLINE static FString GetCommonElipsis()
	{
		// Calculate the number of dots based on time (1 to 3 dots cycling every 3 seconds)
		int32 NumDots = (static_cast<int32>(FPlatformTime::Seconds()) % 3) + 1;
		FString Elipsis =
			FString::Printf(TEXT("%.*s"), NumDots, TEXT("..."));
		
		return Elipsis;
	}

private:
	static TSharedRef<class FSlateStyleSet> Create();


	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
