// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphModule.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ThinkGraphStyle.h"
#include "ThinkGraphCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName ThinkGraphTabName("ThinkGraph");

#define LOCTEXT_NAMESPACE "FThinkGraphModule"

void FThinkGraphModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FThinkGraphStyle::Initialize();
	FThinkGraphStyle::ReloadTextures();

	FThinkGraphCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FThinkGraphCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FThinkGraphModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FThinkGraphModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ThinkGraphTabName,
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this, &FThinkGraphModule::OnSpawnPluginTab))
	                        .SetDisplayName(LOCTEXT("FThinkGraphTabTitle", "ThinkGraph"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);

	IModuleInterface::StartupModule();


}

void FThinkGraphModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FThinkGraphStyle::Shutdown();

	FThinkGraphCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ThinkGraphTabName);
}

TSharedRef<SDockTab> FThinkGraphModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FThinkGraphModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ThinkGraph.cpp"))
	);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FThinkGraphModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ThinkGraphTabName);
}

void FThinkGraphModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FThinkGraphCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FThinkGraphCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FThinkGraphModule, ThinkGraph)
