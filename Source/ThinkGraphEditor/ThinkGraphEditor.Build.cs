// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThinkGraphEditor : ModuleRules
{
	public ThinkGraphEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicIncludePaths.AddRange(
        			new string[]
        			{
        				"ThinkGraphEditor/",
        			}
        		);	
        		
        		PrivateIncludePaths.AddRange(
        			new string[] {
        				"ThinkGraphEditor/",
        			}
        		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				
				"Core",
				"ThinkGraph"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"EditorStyle",
				"KismetWidgets",
				"GraphEditor",
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
		
		