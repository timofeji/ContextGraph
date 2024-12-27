// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphEditorModule.h"

#include "AssetTypeActions_ThinkGraph.h"
#include "EdGraphUtilities.h"
#include "IAssetTools.h"
#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorCommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName ThinkGraphTabName("ThinkGraphEditor");

#define LOCTEXT_NAMESPACE "FThinkGraphEditorModule"


static void RefreshSlateStyles()
{
	FThinkGraphEditorStyle::ReloadTextures();
	UE_LOG(LogTemp, Log, TEXT("Slate styles and textures refreshed."));
}

// Register the console command
FAutoConsoleCommand RefreshStyleCommand(
	TEXT("ThinkGraph.RefreshStyles"),
	TEXT("Refreshes all registered Slate styles and forces textures to update."),
	FConsoleCommandDelegate::CreateStatic(RefreshSlateStyles)
);

void FThinkGraphEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FThinkGraphEditorStyle::Initialize();
	FThinkGraphEditorStyle::ReloadTextures();

	FThinkGraphEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FThinkGraphEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FThinkGraphEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ThinkGraphTabName,
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this, &FThinkGraphEditorModule::OnSpawnPluginTab))
	                        .SetDisplayName(LOCTEXT("FThinkGraphEditorTabTitle", "ThinkGraph"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);


	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked
		<FAssetToolsModule>("AssetTools").Get();

	EAssetTypeCategories::Type GameplayAssetCategory = AssetTools.
		RegisterAdvancedAssetCategory(
			FName(TEXT("Gameplay")),
			FText::FromName(TEXT("Gameplay")));

	TSharedPtr<IAssetTypeActions> ThinkGraphType = MakeShareable(new
		FAssetTypeActions_ThinkGraph(GameplayAssetCategory));

	AssetTools.RegisterAssetTypeActions(ThinkGraphType.ToSharedRef());
	ThinkNodeFactory = MakeShareable(new FThinkGraphNodePanelFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(ThinkNodeFactory);
	//
	// FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
	// 	"PropertyEditor");
	// PropertyEditorModule.RegisterCustomClassLayout("ThinkGraphEdNode",
	//                                                FOnGetDetailCustomizationInstance::CreateStatic(
	// 	                                               &FThinkGraphDetails::MakeInstance));
}

void FThinkGraphEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FThinkGraphEditorStyle::Shutdown();

	FThinkGraphEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ThinkGraphTabName);
}

TSharedRef<SDockTab> FThinkGraphEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FThinkGraphEditorModule::OnSpawnPluginTab")),
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

void FThinkGraphEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ThinkGraphTabName);
}

void FThinkGraphEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FThinkGraphEditorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FThinkGraphEditorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FThinkGraphEditorModule, ThinkGraphEditor)
