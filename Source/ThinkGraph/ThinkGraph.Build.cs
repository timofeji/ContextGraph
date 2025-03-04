// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThinkGraph : ModuleRules
{
	public ThinkGraph(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
			PublicIncludePaths.AddRange(
        			new string[]
        			{
        				"ThinkGraph/",
        			}
        		);	
        		
        		PrivateIncludePaths.AddRange(
        			new string[] {
        				"ThinkGraph/",
        			}
        		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Http", "Json", "JsonUtilities",
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
