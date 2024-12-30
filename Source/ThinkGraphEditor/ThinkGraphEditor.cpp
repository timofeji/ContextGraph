// Created by Timofej Jermolaev, All rights reserved . 

#include "ThinkGraphEditor.h"

#include "EdGraphUtilities.h"

#include "EditorViewportCommands.h"
#include "GraphEditorActions.h"
#include "HttpModule.h"
#include "SkeletalRenderPublic.h"
#include "ThinkGraphDebugger.h"
#include "ThinkGraphEditorCommands.h"
#include "ThinkGraphEditorLog.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/ThinkGraphEdGraph.h"
#include "Graph/ThinkGraphSchema.h"
#include "Graph/Nodes/ThinkGraphEdNode.h"
#include "Graph\Nodes\ThinkGraphEdNodeEdge.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "Widgets/Layout/SBox.h"

#include "Animation/AnimMontage.h"
#include "Graph/Nodes/ThinkGraphEdNode_BasePrompt.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ScopedSlowTask.h"

#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_BasePrompt.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

const FName FThinkGraphEditor::DetailsTabID(TEXT("ThinkGraph_Details"));
const FName FThinkGraphEditor::GraphViewportTabID(TEXT("ThinkGraph_Viewport"));

const FName FThinkGraphEditorModes::ThinkGraphEditorMode("ThinkGraphEditor");

#define LOCTEXT_NAMESPACE "FHBThinkGraphAssetEditor"

void FThinkGraphEditor::OnSelectedNodesChanged(const TSet<UObject*>& Objects) const
{
}

void FThinkGraphEditor::OnGraphActionMenuClosed(bool bArg, bool bCond) const
{
}

FActionMenuContent FThinkGraphEditor::OnCreateGraphActionMenu(UEdGraph* EdGraph,
                                                              const UE::Math::TVector2<double>& Vector2,
                                                              const TArray<UEdGraphPin*>& EdGraphPins, bool bArg,
                                                              TDelegate<void()> Delegate)
{
	return FActionMenuContent();
}


FThinkGraphEditor::FThinkGraphEditor()
{
}

FThinkGraphEditor::~FThinkGraphEditor()
{
}

void FThinkGraphEditor::CreateInternalWidgets()
{
	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(
		this, &FThinkGraphEditor::OnNodeSelectionChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FThinkGraphEditor::OnGraphNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FThinkGraphEditor::OnNodeTitleCommitted);

	UEdGraph* EditorGraph = GraphBeingEdited->EditorGraph;
	check(EditorGraph);


	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString(GraphBeingEdited->GetName());

	// Make full graph editor
	const bool bGraphIsEditable = EditorGraph->bEditable;
	GraphEditorView = SNew(SGraphEditor)
		.AdditionalCommands(DefaultCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditorGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(true);
}

void FThinkGraphEditor::ExtendMenu()
{
}

bool FThinkGraphEditor::CanDebug() const
{
	return true;
}


void FThinkGraphEditor::OnChatGPTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) const
{

}
void FThinkGraphEditor::RunDebug() const
{
	

}

void FThinkGraphEditor::ExtendToolbar()
{
	// If the ToolbarExtender is valid, remove it before rebuilding it
	if (ToolbarExtender.IsValid())
	{
		RemoveToolbarExtender(ToolbarExtender);
		ToolbarExtender.Reset();
	}


	FName ParentName;
	static const FName MenuName = GetToolMenuToolbarName(ParentName);

	UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu(MenuName);
	const FToolMenuInsert SectionInsertLocation("Asset", EToolMenuInsertType::After);


	ToolbarExtender = MakeShareable(new FExtender);

	AddToolbarExtender(ToolbarExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& ToolbarBuilder)
			{
				FillToolbar(ToolbarBuilder);


				// FPersonaModule& PersonaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
				// TSharedRef<class IAssetFamily> AssetFamily = PersonaModule.CreatePersonaAssetFamily(
				// 	GraphBeingEdited->SkeletalMesh);
				// AddToolbarWidget(PersonaModule.CreateAssetFamilyShortcutWidget(SharedThis(this), AssetFamily));
			}
		));
}

void FThinkGraphEditor::FillToolbar(FToolBarBuilder& ToolBarBuilder)
{
	const UThinkGraphEdGraph* EditorGraph = Cast<UThinkGraphEdGraph>(
		GraphBeingEdited->EditorGraph);
	const TSharedRef<SWidget> SelectionBox = SNew(SComboButton)
	   		.OnGetMenuContent(this, &FThinkGraphEditor::OnGetDebuggerActorsMenu)
	   		.ButtonContent()
	[
		SNew(STextBlock)
	   			.ToolTipText(LOCTEXT("SelectDebugActor", "Pick actor to debug"))
	   			.Text(EditorGraph && EditorGraph->Debugger.IsValid()
		                  ? FText::FromString(EditorGraph->Debugger->GetDebuggedInstanceDesc())
		                  : FText::GetEmpty())
	];


	ToolBarBuilder.BeginSection("World");
	{
		ToolBarBuilder.AddToolBarButton(FUIAction(
			                                FExecuteAction::CreateSP(this, &FThinkGraphEditor::RunDebug),
			                                FCanExecuteAction::CreateSP(this, &FThinkGraphEditor::CanDebug))
		                                , NAME_None
		                                , LOCTEXT("PrevConflictLabel", "Debug")
		                                , LOCTEXT("PrevConflictTooltip", "Runs current graph")
		                                , FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintMerge.NextDiff")
		);
		ToolBarBuilder.AddSeparator();
		ToolBarBuilder.AddWidget(SelectionBox);
	}
	ToolBarBuilder.EndSection();
}


TSharedRef<SWidget> FThinkGraphEditor::OnGetDebuggerActorsMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	const auto* EditorGraph = Cast<UThinkGraphEdGraph>(GraphBeingEdited->EditorGraph);
	if (!EditorGraph)
	{
		return MenuBuilder.MakeWidget();
	}

	if (EditorGraph && EditorGraph->Debugger.IsValid())
	{
		TSharedPtr<FThinkGraphDebugger> Debugger = EditorGraph->Debugger;

		TArray<AActor*> MatchingActors;
		TArray<UThinkGraphComponent*> MatchingInstances;
		Debugger->GetMatchingInstances(MatchingInstances, MatchingActors);

		// Fill the combo menu with actors that started the combo graph once
		for (int32 i = 0; i < MatchingActors.Num(); i++)
		{
			if (MatchingActors[i])
			{
				const FText ActorDesc = FText::FromString(Debugger->DescribeInstance(*MatchingActors[i]));
				TWeakObjectPtr<AActor> InstancePtr = MatchingActors[i];

				FUIAction ItemAction(FExecuteAction::CreateLambda([InstancePtr, EditorGraph]()
				{
					EditorGraph->Debugger->OnInstanceSelectedInDropdown(InstancePtr.Get());
				}));
				MenuBuilder.AddMenuEntry(ActorDesc, TAttribute<FText>(),
				                         FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.AnimMontage"),
				                         ItemAction);
			}
		}

		// Failsafe when no actor match
		if (MatchingActors.Num() == 0)
		{
			const FText ActorDesc = LOCTEXT("NoMatchForDebug", "Can't find matching actors");
			TWeakObjectPtr<AActor> InstancePtr;

			FUIAction ItemAction(FExecuteAction::CreateLambda([InstancePtr, EditorGraph]()
			{
				EditorGraph->Debugger->OnInstanceSelectedInDropdown(InstancePtr.Get());
			}));
			MenuBuilder.AddMenuEntry(ActorDesc, TAttribute<FText>(), FSlateIcon(), ItemAction);
		}
	}

	return MenuBuilder.MakeWidget();
}

bool FThinkGraphEditor::IsPIESimulating() const
{
	return GEditor->bIsSimulatingInEditor || GEditor->PlayWorld;
}

void FThinkGraphEditor::CreateDefaultCommands()
{
	if (DefaultCommands.IsValid())
	{
		return;
	}

	DefaultCommands = MakeShareable(new FUICommandList);

	// Common generic commands
	DefaultCommands->MapAction(
		FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanSelectAllNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanDeleteNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanCopyNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanCutNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanPasteNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanDuplicateNodes)
	);

	DefaultCommands->MapAction(
		FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FThinkGraphEditor::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FThinkGraphEditor::CanRenameNodes)
	);

	DefaultCommands->MapAction(
		FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateRaw(this, &FThinkGraphEditor::OnCreateComment),
		FCanExecuteAction::CreateRaw(this, &FThinkGraphEditor::CanCreateComment)
	);
}

void FThinkGraphEditor::BindToolkitCommands()
{
	FThinkGraphEditorCommands::Register();
	// Auto Arrange commands
	// ToolkitCommands->MapAction(
	// 	FHBThinkGraphBlueprintEditorCommands::Get().AutoArrange,
	// 	FExecuteAction::CreateSP(this, &FThinkGraphEditor::HandleAutoArrange),
	// 	FCanExecuteAction::CreateSP(this, &FThinkGraphEditor::CanAutoArrange)
	// );
	//
	// ToolkitCommands->MapAction(
	// 	FHBThinkGraphBlueprintEditorCommands::Get().AutoArrangeVertical,
	// 	FExecuteAction::CreateSP(this, &FThinkGraphEditor::SetAndHandleAutoArrange, EHBActioGraphAutoArrangeStrategy::Vertical),
	// 	FCanExecuteAction::CreateSP(this, &FThinkGraphEditor::CanAutoArrange)
	// );

	// ToolkitCommands->MapAction(
	// 	FHBCharacterAssetEditorCommands::Get().RegenerateActionTracers,
	// 	FExecuteAction::CreateSP(this, &FThinkGraphEditor::RegenerateActionTracers),
	// 	FCanExecuteAction::CreateSP(this, &FThinkGraphEditor::CanRegenerateTracers)
	// );
}

void FThinkGraphEditor::InitThinkGraphEditor(EToolkitMode::Type Mode,
                                             const TSharedPtr<IToolkitHost>&
                                             InitToolkitHost,
                                             UThinkGraph* GraphToEdit)
{
	check(GraphToEdit);
	GraphBeingEdited = GraphToEdit;


	CreateDefaultCommands();
	BindToolkitCommands();


	FEditorViewportCommands::Register();
	FGenericCommands::Register();
	FGraphEditorCommands::Register();


	CreateEditorGraph();
	CreateInternalWidgets();
	CreatePropertyWidget();


	// Default layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout =
		FTabManager::NewLayout("Standalone_ThinkGraphEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				// Toolbar
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				// #if NY_ENGINE_VERSION < 500
				// 				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				// #endif
				->SetHideTabWell(true)
			)
			->Split(
				// Main Application area
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split(
					// Left
					// Details tab
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.8f)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->AddTab(GraphViewportTabID,
						         ETabState::OpenedTab)
					)
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.2f)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->AddTab(DetailsTabID,
						         ETabState::OpenedTab)
					)

				)

			)
		);


	// Initialize the asset editor and spawn nothing (dummy layout)
	constexpr bool bCreateDefaultStandaloneMenu = true;
	constexpr bool bCreateDefaultToolbar = true;
	constexpr bool bInIsToolbarFocusable = false;

	FAssetEditorToolkit::InitAssetEditor(Mode,
	                                     InitToolkitHost,
	                                     FName(TEXT("ThinkGraphEditor")),
	                                     // FTabManager::FLayout::NullLayout,
	                                     StandaloneDefaultLayout,
	                                     bCreateDefaultStandaloneMenu,
	                                     bCreateDefaultToolbar,
	                                     GraphBeingEdited,
	                                     bInIsToolbarFocusable);


	UThinkGraphEdGraph* HBActioGraphEd = Cast<UThinkGraphEdGraph>(GraphBeingEdited->EditorGraph);
	check(HBActioGraphEd);

	HBActioGraphEd->Debugger = MakeShareable(new FThinkGraphDebugger);
	HBActioGraphEd->Debugger->Setup(GraphBeingEdited, SharedThis(this));


	RebuildThinkGraph();

	// extend menus and toolbar
	ExtendMenu();
	ExtendToolbar();
	RegenerateMenusAndToolbars();
}


TSharedRef<SDockTab> FThinkGraphEditor::SpawnTab_Details(const FSpawnTabArgs& SpawnTabArgs) const
{
	check(SpawnTabArgs.GetTabId() == DetailsTabID);

	// TODO use DialogueEditor.Tabs.Properties
	const auto* IconBrush = FAppStyle::GetBrush(TEXT("GenericEditor.Tabs.Properties"));


	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
	.Label(LOCTEXT("ThinkGraphDetailsTitle",
	               "Details"))
	.TabColorScale(GetTabColorScale())
	[
		DetailsBorder.ToSharedRef()
	];

	NewTab->SetTabIcon(IconBrush);

	return NewTab;
}

FEdGraphPinType FThinkGraphEditor::GetTargetPinType() const
{
	// void* ValuePtr = nullptr;
	// if (TypePropertyHandle->GetValueData(ValuePtr) != FPropertyAccess::Fail)
	// {
	// 	return *((FEdGraphPinType*)ValuePtr);
	// }

	return FEdGraphPinType();
}

void FThinkGraphEditor::HandlePinTypeChanged(const FEdGraphPinType& InPinType)
{
	void* ValuePtr = nullptr;
	if (TypePropertyHandle->GetValueData(ValuePtr) != FPropertyAccess::Fail)
	{
		TypePropertyHandle->NotifyPreChange();

		*((FEdGraphPinType*)ValuePtr) = InPinType;

		TypePropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		//
		// UAnimBlueprint* AnimBlueprint = WeakOuterNode->GetAnimBlueprint();
		// IAssetEditorInstance* AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(
		// 	AnimBlueprint, false);
		// check(AssetEditor->GetEditorName() == "AnimationBlueprintEditor");

		LastGraphPinType = InPinType;
	}
}

void FThinkGraphEditor::DebuggerUpdateGraph(bool bIsPIEActive)
{
	if (bIsPIEActive)
	{
		RebuildThinkGraph();
	}
}


TSharedRef<SDockTab> FThinkGraphEditor::SpawnTab_GraphViewport(const FSpawnTabArgs& SpawnTabArgs)
{
	check(SpawnTabArgs.GetTabId() == FThinkGraphEditor::GraphViewportTabID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ThinkGraphViewportTab_Title", "GraphView"));

	if (GraphEditorView.IsValid())
	{
		SpawnedTab->SetContent(GraphEditorView.ToSharedRef());
	}

	return SpawnedTab;
}


void FThinkGraphEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT(
		"WorkspaceMenu_ThinkGraphEditor",
		"HBCharacter Editor"));
	TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.
		ToSharedRef();
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FThinkGraphEditor::GraphViewportTabID,
	                                 FOnSpawnTab::CreateSP(this, &FThinkGraphEditor::SpawnTab_GraphViewport))
	            .SetDisplayName(LOCTEXT("ThinkGraphViewportTab", "GraphViewport"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));


	InTabManager->RegisterTabSpawner(FThinkGraphEditor::DetailsTabID,
	                                 FOnSpawnTab::CreateSP(this, &FThinkGraphEditor::SpawnTab_Details))
	            .SetDisplayName(LOCTEXT("ThinkGraphDetailsTab", "PropertyDetails"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FThinkGraphEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FText FThinkGraphEditor::GetBaseToolkitName() const
{
	return LOCTEXT("HBCharacterEditorAppLabel", "HB Character Editor");
}

FText FThinkGraphEditor::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	check(EditingObjs.Num() > 0);

	const UObject* EditingObject = EditingObjs[0];
	check(EditingObject);

	FFormatNamedArguments Args;
	Args.Add(TEXT("ThinkGraphName"), FText::FromString(EditingObject->GetName()));
	Args.Add(TEXT("DirtyState"), EditingObject->GetOutermost()->IsDirty()
		                             ? FText::FromString(TEXT("*"))
		                             : FText::GetEmpty());
	return FText::Format(LOCTEXT("ThinkGraphAssetEditorToolkitName", "{ThinkGraphName}{DirtyState}"), Args);
}

void FThinkGraphEditor::RebuildThinkGraph()
{
	if (!GraphBeingEdited || !GraphBeingEdited)
	{
		return;
	}

	if (auto Graph{CastChecked<UThinkGraphEdGraph>(GraphBeingEdited->EditorGraph)})
	{
		Graph->RebuildGraph();
	}
}

void FThinkGraphEditor::SaveAsset_Execute()
{
	if (GraphBeingEdited)
	{
		RebuildThinkGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FThinkGraphEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	// if (GraphBeingEdited)
	// {
	// 	GraphBeingEdited->EditorGraph->GetSchema()->ForceVisualizationCacheClear();
	// }
}

FSlateColor FThinkGraphEditor::GetDetailsBorderColor() const
{
	uint8 H = static_cast<uint8>(FMath::Fmod(FPlatformTime::Seconds() * 25.5f, 255.0f));
	return FLinearColor::MakeFromHSV8(H, 255, 255);

	// return FLinearColor::Green;
}

void FThinkGraphEditor::CreatePropertyWidget()
{
	FDetailsViewArgs Args;
	Args.bUpdatesFromSelection = false;
	Args.bLockable = false;
	Args.bAllowSearch = true;
	Args.NameAreaSettings = FDetailsViewArgs::ActorsUseNameArea;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;
	Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Show;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView = PropertyModule.CreateDetailView(Args);
	DetailsView->SetObject(GraphBeingEdited);
	DetailsView->OnFinishedChangingProperties().AddSP(
		this, &FThinkGraphEditor::OnFinishedChangingProperties);


	// FSlateBrush CustomBrush;
	// CustomBrush.DrawAs = ESlateBrushDrawType::Box;
	// CustomBrush.Margin = FMargin(20.f); // Controls border width
	// CustomBrush.TintColor = FSlateColor(FLinearColor::White); // Optional tint
	//
	SAssignNew(DetailsBorder, SBorder)
	.Padding(FMargin(2.f))
							.BorderImage( FAppStyle::GetBrush("ErrorReporting.Box") )
					// .BorderImage(FAppStyle::GetBrush("NotificationList.ItemBackground"))
	// .BorderImage(&CustomBrush)
	.BorderBackgroundColor(this, &FThinkGraphEditor::GetDetailsBorderColor);
	DetailsBorder->SetContent(DetailsView.ToSharedRef());
}

void FThinkGraphEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Collector.AddReferencedObject(GraphBeingEdited->EditorGraph);
	// Collector.AddReferencedObject(GraphBeingEdited);
	Collector.AddReferencedObject(GraphBeingEdited);
}

void FThinkGraphEditor::CreateEditorGraph()
{
	if (!GraphBeingEdited)
	{
		return;
	}

	if (!GraphBeingEdited)
	{
		GraphBeingEdited = NewObject<UThinkGraph>(GraphBeingEdited,
		                                          UThinkGraph::StaticClass(),
		                                          FName("ThinkGraph"));
	}

	if (!GraphBeingEdited->EditorGraph)
	{
		// State Machine Graph
		GraphBeingEdited->EditorGraph = FBlueprintEditorUtils::CreateNewGraph(
			GraphBeingEdited,
			FName("ThinkEdGraph"),
			UThinkGraphEdGraph::StaticClass(),
			UThinkGraphSchema::StaticClass()
		);
		GraphBeingEdited->EditorGraph->bAllowRenaming = false;
		GraphBeingEdited->EditorGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the entry node)
		const UEdGraphSchema* GraphSchema = GraphBeingEdited->EditorGraph->GetSchema();
		GraphSchema->CreateDefaultNodesForGraph(*GraphBeingEdited->EditorGraph);
	}
}


FGraphPanelSelectionSet FThinkGraphEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (GraphEditorView.IsValid())
	{
		CurrentSelection = GraphEditorView->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FThinkGraphEditor::SelectAllNodes() const
{
	if (GraphEditorView.IsValid())
	{
		GraphEditorView->SelectAllNodes();
	}
}

bool FThinkGraphEditor::CanSelectAllNodes()
{
	return true;
}

void FThinkGraphEditor::DeleteSelectedNodes() const
{
	if (!GraphEditorView.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	GraphEditorView->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = GraphEditorView->GetSelectedNodes();
	GraphEditorView->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (!EdNode || !EdNode->CanUserDeleteNode())
		{
			continue;
		}

		if (UThinkGraphEdNode* ThinkGraphEdNode = Cast<UThinkGraphEdNode>(EdNode))
		{
			ThinkGraphEdNode->Modify();

			const UEdGraphSchema* Schema = ThinkGraphEdNode->GetSchema();
			if (Schema)
			{
				Schema->BreakNodeLinks(*ThinkGraphEdNode);
			}

			ThinkGraphEdNode->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FThinkGraphEditor::CanDeleteNodes() const
{
	if (!GraphEditorView.IsValid())
	{
		return false;
	}

	if (IsPIESimulating())
	{
		return false;
	}

	const FGraphPanelSelectionSet SelectedNodes = GraphEditorView->GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FThinkGraphEditor::DeleteSelectedDuplicateNodes() const
{
	if (!GraphEditorView.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = GraphEditorView->GetSelectedNodes();
	GraphEditorView->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			GraphEditorView->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicate-able nodes
	DeleteSelectedNodes();

	GraphEditorView->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			GraphEditorView->SetNodeSelection(Node, true);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FThinkGraphEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicateNodes();
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool FThinkGraphEditor::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FThinkGraphEditor::CopySelectedNodes() const
{
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;
	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		if (const UThinkGraphEdNodeEdge* ThinkGraphEdNodeEdge = Cast<UThinkGraphEdNodeEdge>(*SelectedIter))
		{
			const UThinkGraphEdNode* StartNode = ThinkGraphEdNodeEdge->GetStartNodeAsGraphNode();
			const UThinkGraphEdNode* EndNode = ThinkGraphEdNodeEdge->GetEndNode();

			if (!SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	// FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FThinkGraphEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FThinkGraphEditor::PasteNodes()
{
	if (GraphEditorView.IsValid())
	{
		PasteNodesHere(GraphEditorView->GetPasteLocation());
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FThinkGraphEditor::PasteNodesHere(const FVector2D& Location)
{
	if (!GraphEditorView.IsValid())
	{
		return;
	}

	UEdGraph* EdGraph = GraphEditorView->GetCurrentGraph();

	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		GraphEditorView->ClearSelectionSet();

		FString TextToImport;
		// FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			const UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		const float InvNumNodes = 1.0f / (PastedNodes.Num() == 0 ? 1 : PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			GraphEditorView->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	GraphEditorView->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		// ReSharper disable once CppExpressionWithoutSideEffects
		GraphOwner->MarkPackageDirty();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool FThinkGraphEditor::CanPasteNodes()
{
	if (!GraphEditorView.IsValid())
	{
		return false;
	}

	if (IsPIESimulating())
	{
		return false;
	}

	FString ClipboardContent;
	// FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(GraphEditorView->GetCurrentGraph(), ClipboardContent);
}

void FThinkGraphEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool FThinkGraphEditor::CanDuplicateNodes()
{
	return CanCopyNodes();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FThinkGraphEditor::OnRenameNode()
{
	if (GraphEditorView.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode && SelectedNode->bCanRenameNode)
			{
				GraphEditorView->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}

bool FThinkGraphEditor::CanRenameNodes() const
{
	if (!GraphBeingEdited)
	{
		return false;
	}

	if (IsPIESimulating())
	{
		return false;
	}

	// UThinkGraphEdGraph* HBActioGraphEd = Cast<UThinkGraphEdGraph>(GraphBeingEdited->EditorGraph);
	// check(HBActioGraphEd);

	return GetSelectedNodes().Num() == 1;
}

bool FThinkGraphEditor::CanCreateComment() const
{
	return GraphEditorView.IsValid() ? (GraphEditorView->GetNumberOfSelectedNodes() != 0) : false;
}

void FThinkGraphEditor::OnCreateComment() const
{
	if (UEdGraph* EdGraph = GraphEditorView.IsValid() ? GraphEditorView->GetCurrentGraph() : nullptr)
	{
		const TSharedPtr<FEdGraphSchemaAction> Action = EdGraph->GetSchema()->GetCreateCommentAction();
		if (Action.IsValid())
		{
			Action->PerformAction(EdGraph, nullptr, FVector2D());
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FThinkGraphEditor::OnNodeSelectionChanged(const TSet<UObject*>& NewSelection)
{
	TG_ERROR(Verbose, TEXT("OnNodeSelectionChanged - %d"), NewSelection.Num())

	TArray<UObject*> SelectedNodes;
	TArray<UThinkGraphEdNode*> GraphNodes;
	for (UObject* Selection : NewSelection)
	{
		SelectedNodes.Add(Selection);

		if (UThinkGraphEdNode* Node = Cast<UThinkGraphEdNode>(Selection))
		{
			GraphNodes.Add(Node);
		}
	}


	FSlateFontInfo FontInfo = FThinkGraphEditorStyle::Get().GetFontStyle("ThinkGraph.Text.Prompt");
	FontInfo.Size = 10;


	if (GraphNodes.Num() > 0)
	{
		if (auto PromptEdNode = Cast<UThinkGraphEdNode_BasePrompt>(GraphNodes[0]))
		{
			auto PromptNode = Cast<UThinkGraphNode_BasePrompt>(PromptEdNode->RuntimeNode);
			auto TextEdit =
				SNew(SMultiLineEditableTextBox)
				.Font(FontInfo)
				.BackgroundColor(FLinearColor::Black)
				.ForegroundColor(FLinearColor(FColor::FromHex("5b8c3f")))
				.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
				.AllowMultiLine(true)
				.Margin(0.f)
				.Text_Lambda([PromptNode]() { return PromptNode->Prompt; })
				.OnTextCommitted_Lambda([PromptNode](const FText& NewText, ETextCommit::Type CommitType)
				                               {
					                               PromptNode->Prompt = NewText;
				                               });

			DetailsBorder->SetContent(TextEdit);

			return;
		}
		DetailsView->SetObjects(SelectedNodes);
	}
	else
	{
		DetailsView->SetObject(GraphBeingEdited);
	}


	DetailsBorder->SetContent(DetailsView->AsShared());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FThinkGraphEditor::OnGraphNodeDoubleClicked(UEdGraphNode* Node)
{
	TG_ERROR(Verbose, TEXT("OnGraphNodeDoubleClicked - %s"), *GetNameSafe(Node))

	// TODO: Handle rename of node on double click
	// or opening of animation editor
}

void FThinkGraphEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo,
                                             UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		static const FText TransactionTitle = FText::FromString(FString(TEXT("Rename Node")));
		const FScopedTransaction Transaction(TransactionTitle);
		NodeBeingChanged->OnRenameNode(NewText.ToString());
		NodeBeingChanged->Modify();
	}
}

#undef  LOCTEXT_NAMESPACE
