// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Memory.h"

class UThinkGraphBufferEditor;
class IHttpResponse;
class IHttpRequest;
class UThinkGraph;
class UThinkGraphNode_Parse;
class UThinkGraphNode_LLM;

class AHBWeapon;



struct FThinkGraphEditorModes
{
	// Mode constants
	static const FName ThinkGraphEditorMode;

	static FText GetLocalizedMode(const FName InMode)
	{
		static TMap<FName, FText> LocModes;

		if (LocModes.Num() == 0)
		{
			LocModes.Add(ThinkGraphEditorMode, NSLOCTEXT("HBCombatEditorModes", "ThinkGraphEditorMode", "Combat"));
		}

		check(InMode != NAME_None);
		const FText* OutDesc = LocModes.Find(InMode);
		check(OutDesc);
		return *OutDesc;
	}

private:
	FThinkGraphEditorModes()
	{
	}
};


class FThinkGraphEditor : public FAssetEditorToolkit, public FGCObject, public FNotifyHook
{
public:
	FThinkGraphEditor();
	~FThinkGraphEditor();
	void CreateInternalWidgets();

	void OnSelectedNodesChanged(const TSet<UObject*>& Objects) const;
	void OnNodeTitleCommitted(const FText& Text, ETextCommit::Type Arg, UEdGraphNode* EdGraphNode) const;
	void OnGraphActionMenuClosed(bool bArg, bool bCond) const;

	FActionMenuContent OnCreateGraphActionMenu(UEdGraph* EdGraph, const UE::Math::TVector2<double>& Vector2,
	                                           const TArray<UEdGraphPin*>& EdGraphPins,
	                                           bool bArg, TDelegate<void()> Delegate);
	

	void OnNodeSelectionChanged(const TSet<UObject*>& Objects);

	void OnGraphNodeDoubleClicked(UEdGraphNode* EdGraphNode);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	void CreateEditorGraph();


	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override
	{
		return TEXT("FHBCharacterAssetEditor");
	}

	void ExtendMenu();

	bool CanDebug() const;
	UFUNCTION()
	void OnChatGPTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) const;
	
	void RunDebug() const;
	void FillToolbar(FToolBarBuilder& ToolBarBuilder);
	void ExtendToolbar();
	bool IsPIESimulating() const;
	void CreateDefaultCommands();
	void BindThinkGraphCommands();

	bool CanFind();
	void Find();
	
	void InitThinkGraphEditor(EToolkitMode::Type Mode,
	                         const TSharedPtr<IToolkitHost>& InitToolkitHost,
	                         UThinkGraph* GraphToEdit);


	FGraphPanelSelectionSet GetSelectedNodes() const;


	TSharedRef<SWidget> OnGetDebuggerActorsMenu();

	
	
	void SelectAllNodes() const;
	bool CanSelectAllNodes();
	void DeleteSelectedNodes() const;
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicateNodes() const;
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes() const;
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes();
	void DuplicateNodes();
	bool CanDuplicateNodes();
	void OnRenameNode();
	bool CanRenameNodes() const;

	bool CanCreateComment() const;
	void OnCreateComment() const;
	
	//*Tracers
	

	TSharedPtr<class FBlueprintEditorToolbar> Toolbar;
	

	float SampleRate = 20.f;
	float TracerAnimTime = 0.1;
	FTimerHandle SampleTimerHandle;
	TSharedPtr<SBorder> DetailsBorder;
	
	TObjectPtr<UThinkGraphNode_Memory> DebuggedNode;

	TSharedPtr<class FBlueprintEditorToolbar> GetToolbarBuilder() { return Toolbar; }


	static const FName ViewportTabID;
	static const FName GraphViewportTabID;
	static const FName ActionNameTabID;
	static const FName DetailsTabID;
	static const FName MemoryTabID;
	static const FName PaletteTabID;


	// // Refreshes the graph viewport.
	// void RefreshViewport() const
	// {
	// 	if (GraphEditorView.IsValid())
	// 	{
	// 		GraphEditorView->NotifyGraphChanged();
	// 	}
	// }

	// IToolkit interface
	//

	bool CanAddValueBindPin();
	void OnAddValueBindPin();
	bool CanRemoveValueBindPin();
	void OnRemoveValueBindPin();
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& SpawnTabArgs) const;
	TSharedRef<SDockTab>  SpawnTab_Memory(const FSpawnTabArgs& SpawnTabArgs) const;
	FEdGraphPinType GetTargetPinType() const;
	void HandlePinTypeChanged(const FEdGraphPinType& EdGraphPin);
	TSharedRef<SDockTab> SpawnTab_GraphViewport(const FSpawnTabArgs& SpawnTabArgs);

	void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	FText GetBaseToolkitName() const override;
	FText GetToolkitName() const override;
	FName GetToolkitFName() const override { return FName(TEXT("ThinkGraphEditor")); }

	FText GetToolkitToolTipText() const override
	{
		return GetToolTipTextForObject(Cast<UObject>(GraphBeingEdited));
	}

	FLinearColor GetWorldCentricTabColorScale() const override
	{
		return FLinearColor::White;
	}

	FString GetWorldCentricTabPrefix() const override
	{
		return FString(TEXT("HBCollectibleEditor"));
	}


	void RebuildThinkGraph();

	virtual void SaveAsset_Execute() override;

	/** The command list for this asset editor with common functionality such as copy paste and so on */
	TSharedPtr<FUICommandList> DefaultCommands;
	TSharedPtr<FUICommandList> EditorCommands;

	/** Toolbar extender */
	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<IPropertyHandle> TypePropertyHandle;
	FEdGraphPinType LastGraphPinType;
	int ActionIndex = 0;
	TSharedPtr<UThinkGraphBufferEditor> BufferEditorWindow;



	AStaticMeshActor* WeakGroundActorPtr;



	/** Called from Editor debugger and rebuilds exec graph for the Combo Graph Asset, and indicates if debugger is in active state (eg. PIE running) */
	void DebuggerUpdateGraph(bool bIsPIEActive);

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	
	FSlateColor GetDetailsBorderColor() const;
	
	void CreatePropertyWidget();

	// The custom details view used
	TSharedPtr<IDetailsView> DetailsView;

	TSharedPtr<SGraphEditor> GraphEditorView;


	/** Get the Graph Editor Widget */
	TSharedPtr<SGraphEditor> GetGraphEditorWidget() const { return GraphEditorView; }
	TSharedPtr<IDetailsView> GetPropertyDetailsWidget() const { return DetailsView; }

protected:
	UThinkGraph* GraphBeingEdited;
	
};
