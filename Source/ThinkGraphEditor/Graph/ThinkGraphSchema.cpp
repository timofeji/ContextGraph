#include "ThinkGraphSchema.h"

#include "ConnectionDrawingPolicy.h"
#include "EdGraphNode_Comment.h"
#include "ThinkGraphConnectionDrawingPolicy.h"
#include "ThinkGraphEditorLog.h"
#include "ThinkGraphEditorTypes.h"
// #include "ThinkGraph/ThinkGraphNode_Entry.h"
#include "Animation/AnimMontage.h"
#include "Nodes/ThinkGraphEdNode_Const.h"
#include "Nodes/ThinkGraphEdNode_Stimulus.h"
#include "Nodes\ThinkGraphEdNode_Memory.h"
#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/ThinkGraphEdge.h"
#include "Slate/SThinkGraphNode.h"

#include "Nodes\ThinkGraphEdNode.h"
#include "Nodes\ThinkGraphEdNodeEdge.h"
#include "Nodes\ThinkGraphEdNode_LLM.h"
#include "Nodes\ThinkGraphEdNode_Parse.h"
#include "..\..\ThinkGraph\Nodes\TGNode.h"
#include "Nodes/ThinkGraphEdNode_Embed.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Const.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Embed.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Memory.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Stimulus.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_LLM.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Parse.h"

#define LOCTEXT_NAMESPACE "ThinkGraphSchema"

int32 UThinkGraphSchema::CurrentCacheRefreshID = 0;

class FACENodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{
		VisitedNodes.Add(StartNode);
		return TraverseNodes(EndNode);
	}

private:
	TSet<UEdGraphNode*> VisitedNodes;
	TSet<UEdGraphNode*> FinishedNodes;

	bool TraverseNodes(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (UEdGraphPin* MyPin : Node->Pins)
		{
			if (MyPin->Direction == EGPD_Output)
			{
				for (const UEdGraphPin* OtherPin : MyPin->LinkedTo)
				{
					UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
					if (VisitedNodes.Contains(OtherNode))
					{
						// Only  an issue if this is a back-edge
						return false;
					}

					if (!FinishedNodes.Contains(OtherNode) && !TraverseNodes(OtherNode))
					{
						// Only should traverse if this node hasn't been traversed
						return false;
					}
				}
			}
		}

		VisitedNodes.Remove(Node);
		FinishedNodes.Add(Node);
		return true;
	};
};

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_AddComment

UEdGraphNode* FThinkGraphSchemaAction_AddComment::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;

	const TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(ParentGraph);
	if (GraphEditorPtr.IsValid() && GraphEditorPtr->GetBoundsForSelectedNodes(/*out*/ Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(
		ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode

UEdGraphNode* FThinkGraphSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                             const FVector2D Location, const bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("ThinkGraphEditorNewNode", "Think Graph Editor: New Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->RuntimeNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FThinkGraphSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewEdge

UEdGraphNode* FThinkGraphSchemaAction_NewEdge::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                             const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("ThinkGraphEditorNewEdge", "Think Graph Editor: New Edge"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->RuntimeEdge->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FThinkGraphSchemaAction_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNodeAction
UEdGraphNode* FThinkGraphSchemaAction_NewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                   const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("ThinkGraphEditorNewAction", "Think Graph Editor: New Action"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}

void FThinkGraphSchemaAction_NewNodeAction::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Stimulus
UEdGraphNode* FThinkGraphSchemaAction_NewNode_Stimulus::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                      const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New Entry Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_Stimulus::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Stimulus


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Memory
UEdGraphNode* FThinkGraphSchemaAction_NewNode_Memory::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                    const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New Entry Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_Memory::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Memory


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Parse
UEdGraphNode* FThinkGraphSchemaAction_NewNode_Parse::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                   const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New Entry Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_Parse::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Parse


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Embed
UEdGraphNode* FThinkGraphSchemaAction_NewNode_Embed::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                   const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New Entry Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_Embed::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Embed

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Const
UEdGraphNode* FThinkGraphSchemaAction_NewNode_Const::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                   const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New Entry Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_Const::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_Const


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_LLM
UEdGraphNode* FThinkGraphSchemaAction_NewNode_LLM::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                 const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	// If there is a template, we actually use it
	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("New LLM Node", "Think Graph Editor: New Entry"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}


void FThinkGraphSchemaAction_NewNode_LLM::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_NewNode_LLM


/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_AutoArrangeVertical

UEdGraphNode* FThinkGraphSchemaAction_AutoArrangeVertical::PerformAction(
	UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UThinkGraphEdGraph* Graph = Cast<UThinkGraphEdGraph>(ParentGraph);
	if (Graph)
	{
		TG_ERROR(Verbose, TEXT("FThinkGraphSchemaAction_AutoArrangeVertical::PerformAction"))
		Graph->AutoArrange(true);
	}

	return nullptr;
}

/////////////////////////////////////////////////////
// FThinkGraphSchemaAction_AutoArrangeHorizontal

UEdGraphNode* FThinkGraphSchemaAction_AutoArrangeHorizontal::PerformAction(
	UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UThinkGraphEdGraph* Graph = Cast<UThinkGraphEdGraph>(ParentGraph);
	if (Graph)
	{
		TG_ERROR(Verbose, TEXT("FThinkGraphSchemaAction_AutoArrangeVertical::PerformAction Horizontal"))
		Graph->AutoArrange(false);
	}

	return nullptr;
}


/////////////////////////////////////////////////////
// UThinkGraphSchema

void UThinkGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UThinkGraph* ThinkGraph = Cast<UThinkGraph>(Graph.GetOuter());
	TG_ERROR(Verbose, TEXT("CreateDefaultNodesForGraph - Graph, Outer ThinkGraph: %s"), *GetNameSafe(ThinkGraph))

	// Create the entry/exit tunnels
	FGraphNodeCreator<UThinkGraphEdNode_LLM> NodeCreator(Graph);
	UThinkGraphEdNode_LLM* EntryNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	// // Create runtime node for this editor node. Entry nodes gets a bare bone anim base one as well with blank anim related info.
	// EntryNode->RuntimeNode = NewObject<UThinkGraph>(ThinkGraph, UThinkGraphNode_Entry::StaticClass());

	if (UThinkGraphEdGraph* EdThinkGraph = CastChecked<UThinkGraphEdGraph>(&Graph))
	{
		EdThinkGraph->EntryNodes.Add(EntryNode);
	}
}

EGraphType UThinkGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UThinkGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	const bool bHasNoParent = (ContextMenuBuilder.FromPin == nullptr);

	// First build up non anim related actions
	if (bHasNoParent)
	{
		const TSharedPtr<FThinkGraphSchemaAction_AddComment> Action_AddComment(
			new FThinkGraphSchemaAction_AddComment(
				LOCTEXT("AddComent", "Add Comment"),
				LOCTEXT("AddComent_ToolTip", "Adds comment")
			));

		const TSharedPtr<FThinkGraphSchemaAction_AutoArrangeVertical> ActionAutoArrangeVertical(
			new FThinkGraphSchemaAction_AutoArrangeVertical(
				LOCTEXT("AutoArrangeNodeCategory", "Auto Arrange"),
				LOCTEXT("AutoArrangeVertical", "Auto Arrange (Vertically)"),
				LOCTEXT("AutoArrangeVertically_ToolTip",
				        "Re-arrange graph layout vertically (from top to bottom). Similar to Behavior Trees."),
				0
			));

		const TSharedPtr<FThinkGraphSchemaAction_AutoArrangeHorizontal> ActionAutoArrangeHorizontal(
			new FThinkGraphSchemaAction_AutoArrangeHorizontal(
				LOCTEXT("AutoArrangeNodeCategory", "Auto Arrange"),
				LOCTEXT("AutoArrangeHorizontal", "Auto Arrange (Horizontally)"),
				LOCTEXT("AutoArrangeHorizontally_ToolTip",
				        "Re-arrange graph layout horizontally (from left to right). This is far from perfect, but still handy. Will be revisited and improved upon in a future update."),
				0
			));

		ContextMenuBuilder.AddAction(Action_AddComment);
		ContextMenuBuilder.AddAction(ActionAutoArrangeVertical);
		ContextMenuBuilder.AddAction(ActionAutoArrangeHorizontal);
	}


	const TSharedPtr<FThinkGraphSchemaAction_NewNode_Const> ConstNodeAction(
		new FThinkGraphSchemaAction_NewNode_Const(
			LOCTEXT("ThinkGraphNode_BasePrompt", "Data"),
			LOCTEXT("AddPrompt", "Add Prompt..."),
			LOCTEXT("AddPrompt",
			        "AddPrompt"),
			1
		));
	ConstNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_Const>(ContextMenuBuilder.OwnerOfTemporaries);
	ConstNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_Const>(ConstNodeAction->NodeTemplate);
	ConstNodeAction->NodeTemplate->RuntimeNode->SetNodeTitle(FText::FromString("Base Prompt"));
	ContextMenuBuilder.AddAction(ConstNodeAction);


	const TSharedPtr<FThinkGraphSchemaAction_NewNode_Embed> EmbedNodeAction(
		new FThinkGraphSchemaAction_NewNode_Embed(
			LOCTEXT("ThinkGraphNode_Embed", "Data"),
			LOCTEXT("AddEmbed", "Add Embed Node..."),
			LOCTEXT("AddEmbed",
			        "Add Embed Node"),
			1
		));
	EmbedNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_Embed>(ContextMenuBuilder.OwnerOfTemporaries);
	EmbedNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_Embed>(EmbedNodeAction->NodeTemplate);
	EmbedNodeAction->NodeTemplate->RuntimeNode->SetNodeTitle(FText::FromString("Inject"));
	ContextMenuBuilder.AddAction(EmbedNodeAction);


	const TSharedPtr<FThinkGraphSchemaAction_NewNode_LLM> LLMNodeAction(
		new FThinkGraphSchemaAction_NewNode_LLM(
			LOCTEXT("ThinkGraphNode_LLM", "Model"),
			LOCTEXT("Add LLM", "Add LLM..."),
			LOCTEXT("AddLLM",
			        "AddLLM"),
			2
		));
	LLMNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_LLM>(ContextMenuBuilder.OwnerOfTemporaries);
	LLMNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_LLM>(LLMNodeAction->NodeTemplate);
	LLMNodeAction->NodeTemplate->RuntimeNode->SetNodeTitle(FText::FromString("LLM"));

	if (auto LLMNode = Cast<UThinkGraphNode_LLM>(LLMNodeAction->NodeTemplate->RuntimeNode))
	{
		LLMNode->AddPromptInput(TEXT("System"));
		LLMNode->AddPromptInput(TEXT("User"));
	}
	ContextMenuBuilder.AddAction(LLMNodeAction);


	const TSharedPtr<FThinkGraphSchemaAction_NewNode_Parse> ParseNodeAction(
		new FThinkGraphSchemaAction_NewNode_Parse(
			LOCTEXT("ThinkGraphNode_Parse", "Think Graph"),
			LOCTEXT("Add Parse Node", "Add Parse Node..."),
			LOCTEXT("AddParseNode",
			        "Add Parse Node "),
			1
		));
	ParseNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_Parse>(ContextMenuBuilder.OwnerOfTemporaries);
	ParseNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_Parse>(ParseNodeAction->NodeTemplate);
	ContextMenuBuilder.AddAction(ParseNodeAction);

	const TSharedPtr<FThinkGraphSchemaAction_NewNode_Memory> MemoryNodeAction(
		new FThinkGraphSchemaAction_NewNode_Memory(
			LOCTEXT("ThinkGraphNode_Memory", "Model"),
			LOCTEXT("Add Memory Node", "Add Memory..."),
			LOCTEXT("AddMemoryNode",
			        "Add Memory Node "),
			2
		));
	MemoryNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_Memory>(ContextMenuBuilder.OwnerOfTemporaries);
	MemoryNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_Memory>(MemoryNodeAction->NodeTemplate);
	MemoryNodeAction->NodeTemplate->RuntimeNode->SetNodeTitle(FText::FromString("Memory"));
	ContextMenuBuilder.AddAction(MemoryNodeAction);


	const TSharedPtr<FThinkGraphSchemaAction_NewNode_Stimulus> StimulusNodeAction(
		new FThinkGraphSchemaAction_NewNode_Stimulus(
			LOCTEXT("ThinkGraphNode_Stimulus", "Think Graph"),
			LOCTEXT("Add Stimulus Node", "Add Stimulus..."),
			LOCTEXT("AddStimulusNode",
			        "Add Stimulus Node "),
			1
		));
	StimulusNodeAction->NodeTemplate = NewObject<UThinkGraphEdNode_Stimulus>(ContextMenuBuilder.OwnerOfTemporaries);
	StimulusNodeAction->NodeTemplate->RuntimeNode = NewObject<UThinkGraphNode_Stimulus>(
		StimulusNodeAction->NodeTemplate);
	StimulusNodeAction->NodeTemplate->RuntimeNode->SetNodeTitle(FText::FromString("Stimulus"));
	ContextMenuBuilder.AddAction(StimulusNodeAction);

	UThinkGraph* ThinkGraph = CastChecked<UThinkGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());
	// Now, build up actions for anim nodes
	if (!ThinkGraph || ThinkGraph->NodeType == nullptr)
	{
		return;
	}

	TSet<TSubclassOf<UTGNode>> Visited;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(ThinkGraph->NodeType) && !It->HasAnyClassFlags(CLASS_Abstract) && !Visited.Contains(*It))
		{
			TSubclassOf<UTGNode> NodeType = *It;
			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))
			{
				continue;
			}

			if (NodeType.GetDefaultObject()->ContextMenuName.IsEmpty())
			{
				continue;
			}

			TG_ERROR(Verbose, TEXT("GetGraphContextActions - Create action from %s"), *NodeType->GetName())
			CreateAndAddActionToContextMenu(ContextMenuBuilder, NodeType);
			Visited.Add(NodeType);
		}
	}
}

void UThinkGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
}

FLinearColor UThinkGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	// light grey
	return FLinearColor(1.f, 0.101961f, 0.101961f, 1.0f);
}

// ReSharper disable once CppConstValueFunctionReturnType
const FPinConnectionResponse UThinkGraphSchema::CanCreateConnection(const UEdGraphPin* PinA,
                                                                    const UEdGraphPin* PinB) const
{
	// Either one or the other is invalid
	if (!PinA || !PinB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinInvalid", "Invalid pins"));
	}

	UEdGraphNode* OwningNodeA = PinA->GetOwningNode();
	UEdGraphNode* OwningNodeB = PinB->GetOwningNode();

	// Either one or the other is invalid
	if (!OwningNodeA || !OwningNodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinInvalidNodes", "Invalid nodes"));
	}

	// Make sure the pins are not on the same node
	if (OwningNodeA == OwningNodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
		                              LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}


	UThinkGraphEdNode* ThinkGraphNodeIn = Cast<UThinkGraphEdNode>(OwningNodeA);
	UThinkGraphEdNode* ThinkGraphNodeOut = Cast<UThinkGraphEdNode>(OwningNodeB);

	if (!ThinkGraphNodeOut->RuntimeNode)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid ThinkGraph Node"));
	}

	// // Disallow wiring onto an entry node
	// if (OwningNodeB->IsA(UThinkGraphEdNode_LLM::StaticClass()))
	// {
	// 	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
	// 	                              LOCTEXT("PinErrorToEntryNode",
	// 	                                      "Cannot connect to an entry node (can only create connections **from** entry node)"));
	// }

	if (ThinkGraphNodeOut == nullptr || ThinkGraphNodeIn == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid ThinkGraph Node"));
	}

	// check for cycles
	FACENodeVisitorCycleChecker CycleChecker;
	if (!CycleChecker.CheckForLoop(OwningNodeA, OwningNodeB))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
		                              LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	FText ErrorMessage;
	if (!ThinkGraphNodeIn->RuntimeNode->CanCreateConnectionTo(ThinkGraphNodeOut->RuntimeNode,
	                                                          ThinkGraphNodeIn->GetOutputPin()->LinkedTo.Num(),
	                                                          ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}

	if (!ThinkGraphNodeOut->RuntimeNode->CanCreateConnectionFrom(ThinkGraphNodeIn->RuntimeNode,
	                                                             ThinkGraphNodeOut->GetInputPin()->LinkedTo.Num(),
	                                                             ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes with Transition Input"));
}

bool UThinkGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// return Super::TryCreateConnection(A, B);

	// We don't actually care about the pin, we want the node that is being dragged between
	UThinkGraphEdNode* NodeA = Cast<UThinkGraphEdNode>(A->GetOwningNode());
	UThinkGraphEdNode* NodeB = Cast<UThinkGraphEdNode>(B->GetOwningNode());

	// Check that this edge doesn't already exist
	for (const UEdGraphPin* TestPin : A->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (const UThinkGraphEdNodeEdge* EdNode_Edge = Cast<UThinkGraphEdNodeEdge>(ChildNode))
		{
			ChildNode = EdNode_Edge->GetEndNode();
		}

		if (ChildNode == NodeB)
		{
			return false;
		}
	}

	if (NodeA && NodeB)
	{
		const bool bModified = Super::TryCreateConnection(A, B);
		return bModified;
	}

	return false;
}

bool UThinkGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UThinkGraphEdNode* NodeA = Cast<UThinkGraphEdNode>(A->GetOwningNode());
	UThinkGraphEdNode* NodeB = Cast<UThinkGraphEdNode>(B->GetOwningNode());

	// Are nodes and pins all valid?
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin())
	{
		TG_ERROR(Verbose, TEXT("CreateAutomaticConversionNodeAndConnections failed"));
		return false;
	}

	TG_ERROR(Verbose, TEXT("CreateAutomaticConversionNodeAndConnections ok"));
	CreateEdgeConnection(A, B, NodeA, NodeB);

	return true;
}

FConnectionDrawingPolicy* UThinkGraphSchema::CreateConnectionDrawingPolicy(
	const int32 InBackLayerID, const int32 InFrontLayerID, const float InZoomFactor, const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FThinkGraphConnectionDrawingPolicy(InFrontLayerID + 2, InFrontLayerID, InZoomFactor, InClippingRect,
	                                              InDrawElements, InGraphObj);
}

void UThinkGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));
	Super::BreakNodeLinks(TargetNode);

	// UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(&TargetNode);
	// FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
}

void UThinkGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, const bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));
	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
	// UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(TargetPin.GetOwningNode());
	// FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
}

void UThinkGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));
	Super::BreakSinglePinLink(SourcePin, TargetPin);

	// UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(TargetPin->GetOwningNode());
	// FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
}

UEdGraphPin* UThinkGraphSchema::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName,
                                              const FEdGraphPinType& InSourcePin,
                                              EEdGraphPinDirection InSourcePinDirection) const
{
	const UThinkGraphEdNode* EdGraphNode = Cast<UThinkGraphEdNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
		return EdGraphNode->GetOutputPin();
	case EGPD_Output:
		return EdGraphNode->GetInputPin();
	default:
		return nullptr;
	}
}

bool UThinkGraphSchema::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType,
                                              EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UThinkGraphEdNode>(InTargetNode) != nullptr;
}

bool UThinkGraphSchema::IsCacheVisualizationOutOfDate(const int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UThinkGraphSchema::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UThinkGraphSchema::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

void UThinkGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const
{
	Super::GetGraphDisplayInformation(Graph, DisplayInfo);
	DisplayInfo.Tooltip = LOCTEXT("DisplayInfo_Tooltip", "ThinkGraph");
}

void UThinkGraphSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition,
                                             UEdGraph* Graph) const
{
	Super::DroppedAssetsOnGraph(Assets, GraphPosition, Graph);
	if (Graph != nullptr)
	{
		if (UAnimationAsset* AnimationAsset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets))
		{
			SpawnNodeFromAsset(AnimationAsset, GraphPosition, Graph, nullptr);
		}
	}
}

void UThinkGraphSchema::DroppedAssetsOnNode(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition,
                                            UEdGraphNode* Node) const
{
	Super::DroppedAssetsOnNode(Assets, GraphPosition, Node);

	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(Node);
	if (Asset && GraphNode && GraphNode->RuntimeNode)
	{
		const FScopedTransaction Transaction(LOCTEXT("UpdateNodeWithAsset", "Updating Node with Asset"));
		GraphNode->Modify();
		// GraphNode->RuntimeNode->SetAnimationAsset(Asset);
		GraphNode->GetSchema()->ForceVisualizationCacheClear();
		GraphNode->ReconstructNode();
	}
}

void UThinkGraphSchema::DroppedAssetsOnPin(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition,
                                           UEdGraphPin* Pin) const
{
	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);

	if (Asset && Pin)
	{
		FVector2D FixedOffset = GetFixedOffsetFromPin(Pin);

		// if (const UThinkGraphEdNode* EdGraphNode = Cast<UThinkGraphEdNode>(Pin->GetOwningNode()))
		// {
		// 	if (EdGraphNode->SlateNode)
		// 	{
		// 		const FVector2D Size = EdGraphNode->SlateNode->ComputeDesiredSize(FSlateApplication::Get().GetApplicationScale());
		// 		FixedOffset.X = Size.X + 100.f;
		// 	}
		// }

		SpawnNodeFromAsset(Asset, GraphPosition + FixedOffset, Pin->GetOwningNode()->GetGraph(), Pin);
	}
}

void UThinkGraphSchema::GetAssetsGraphHoverMessage(const TArray<FAssetData>& Assets, const UEdGraph* HoverGraph,
                                                   FString& OutTooltipText, bool& bOutOkIcon) const
{
	Super::GetAssetsGraphHoverMessage(Assets, HoverGraph, OutTooltipText, bOutOkIcon);

	UAnimationAsset* AnimationAsset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	if (AnimationAsset)
	{
		if (UAnimMontage* Montage = FAssetData::GetFirstAsset<UAnimMontage>(Assets))
		{
			bOutOkIcon = true;
			OutTooltipText = LOCTEXT("MontagesInGraphs", "Drop Montage here to create a new node in the Think Graph").
				ToString();
		}
		else if (UAnimSequence* Sequence = FAssetData::GetFirstAsset<UAnimSequence>(Assets))
		{
			bOutOkIcon = true;
			OutTooltipText = LOCTEXT("SequencesInGraphs",
			                         "Drop Sequence here to create a new node in the Think Graph. Montage will be created dynamically from the Anim Sequence.")
				.ToString();
		}
	}
}

void UThinkGraphSchema::GetAssetsPinHoverMessage(const TArray<FAssetData>& Assets, const UEdGraphPin* HoverPin,
                                                 FString& OutTooltipText, bool& bOutOkIcon) const
{
	Super::GetAssetsPinHoverMessage(Assets, HoverPin, OutTooltipText, bOutOkIcon);

	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	if (!Asset || !HoverPin)
	{
		OutTooltipText = TEXT("");
		bOutOkIcon = false;
		return;
	}

	// const bool bDirectionMatch = HoverPin->Direction == EGPD_Input;
	const bool bDirectionMatch = IsHoverPinMatching(HoverPin);

	if (bDirectionMatch)
	{
		UEdGraphNode* OwningNode = HoverPin->GetOwningNode();
		if (UAnimMontage* Montage = Cast<UAnimMontage>(Asset))
		{
			bOutOkIcon = true;
			OutTooltipText = FText::Format(LOCTEXT("MontagesOnPin", "Connect Montage {0} in the Think Graph to {1}"),
			                               FText::FromString(Asset->GetName()),
			                               OwningNode
				                               ? OwningNode->GetNodeTitle(ENodeTitleType::ListView)
				                               : FText::FromName(HoverPin->PinName)
			).ToString();
		}
		else if (UAnimSequence* Sequence = Cast<UAnimSequence>(Asset))
		{
			bOutOkIcon = true;
			OutTooltipText = FText::Format(
				LOCTEXT("SequencesOnPin", "Connect Sequence {0} in the Think Graph to {1}"),
				FText::FromString(Asset->GetName()),
				OwningNode ? OwningNode->GetNodeTitle(ENodeTitleType::ListView) : FText::FromName(HoverPin->PinName)
			).ToString();
		}
	}
	else
	{
		bOutOkIcon = false;
		OutTooltipText = LOCTEXT("AssetPinHoverMessage_Fail",
		                         "Type or direction mismatch; must be wired to a HBAction input").ToString();
	}
}

void UThinkGraphSchema::GetAssetsNodeHoverMessage(const TArray<FAssetData>& Assets, const UEdGraphNode* HoverNode,
                                                  FString& OutTooltipText, bool& bOutOkIcon) const
{
	Super::GetAssetsNodeHoverMessage(Assets, HoverNode, OutTooltipText, bOutOkIcon);

	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	if ((Asset == nullptr) || (HoverNode == nullptr) || !HoverNode->IsA(UEdGraphNode::StaticClass()))
	{
		OutTooltipText = TEXT("");
		bOutOkIcon = false;
		return;
	}

	const bool bCanPlayAsset = SupportNodeClassForAsset(Asset->GetClass(), HoverNode);
	if (bCanPlayAsset)
	{
		bOutOkIcon = true;
		OutTooltipText = FText::Format(
			LOCTEXT("AssetNodeHoverMessage_Success", "Change node to play '{0}'"),
			FText::FromString(Asset->GetName())).ToString();
	}
	else
	{
		bOutOkIcon = false;
		OutTooltipText = FText::Format(
			LOCTEXT("AssetNodeHoverMessage_Fail", "Cannot play '{0}' on this node type"),
			FText::FromString(Asset->GetName())).ToString();
	}
}

bool UThinkGraphSchema::CanDuplicateGraph(UEdGraph* InSourceGraph) const
{
	return false;
}

int32 UThinkGraphSchema::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	if (Graph)
	{
		const TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(Graph);
		if (GraphEditorPtr.IsValid())
		{
			return GraphEditorPtr->GetNumberOfSelectedNodes();
		}
	}

	return 0;
}

TSharedPtr<FEdGraphSchemaAction> UThinkGraphSchema::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FThinkGraphSchemaAction_AddComment));
}

void UThinkGraphSchema::SpawnNodeFromAsset(UAnimationAsset* Asset, const FVector2D& GraphPosition, UEdGraph* Graph,
                                           UEdGraphPin* PinIfAvailable)
{
	check(Graph);
	check(Graph->GetSchema()->IsA(UThinkGraphSchema::StaticClass()));
	check(Asset);

	const bool bDirectionMatch = (PinIfAvailable == nullptr) || IsHoverPinMatching(PinIfAvailable);
	if (bDirectionMatch)
	{
		UClass* NewNodeClass = GetNodeClassForAnimAsset(Asset, Graph);
		UClass* NewRuntimeClass = GetRuntimeClassForAnimAsset(Asset, Graph);
		if (NewNodeClass && NewRuntimeClass)
		{
			TG_ERROR(Verbose, TEXT("SpawnNodeFromAsset - NewNodeClass: %s, NewRuntimeClass: %s"),
			         *NewNodeClass->GetName(), *NewRuntimeClass->GetName());
			UThinkGraphEdNode* NewNode = NewObject<UThinkGraphEdNode>(Graph, NewNodeClass);
			NewNode->RuntimeNode = NewObject<UTGNode>(NewNode, NewRuntimeClass);
			// NewNode->RuntimeNode->SetAnimationAsset(Asset);

			FThinkGraphSchemaAction_NewNode Action;
			Action.NodeTemplate = NewNode;
			Action.PerformAction(Graph, PinIfAvailable, GraphPosition);
		}
	}
}

UClass* UThinkGraphSchema::GetNodeClassForAnimAsset(const UAnimationAsset* Asset, const UEdGraph* Graph)
{
	if (Asset->GetClass()->IsChildOf(UAnimMontage::StaticClass()))
	{
		return UThinkGraphEdNode::StaticClass();
	}

	if (Asset->GetClass()->IsChildOf(UAnimSequence::StaticClass()))
	{
		return UThinkGraphEdNode::StaticClass();
	}

	return nullptr;
}

UClass* UThinkGraphSchema::GetRuntimeClassForAnimAsset(const UAnimationAsset* Asset, const UEdGraph* Graph)
{
	UThinkGraph* ThinkGraph = Cast<UThinkGraph>(Graph->GetOuter());
	//
	// if (Asset->GetClass()->IsChildOf(UAnimMontage::StaticClass()))
	// {
	// 	return HBThinkGraph ? HBThinkGraph->DefaultNodeMontageType : UHBThinkGraphNodeMontage::StaticClass();
	// }
	//
	// if (Asset->GetClass()->IsChildOf(UAnimSequence::StaticClass()))
	// {
	// 	return HBThinkGraph ? HBThinkGraph->DefaultNodeSequenceType : UHBThinkGraphNodeSequence::StaticClass();
	// }

	return nullptr;
}

bool UThinkGraphSchema::SupportNodeClassForAsset(UClass* AssetClass, const UEdGraphNode* InGraphNode)
{
	const UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(InGraphNode);
	if (GraphNode && GraphNode->RuntimeNode)
	{
		// See if this node supports this asset type (primary or not)
		// return GraphNode->RuntimeNode->SupportsAssetClass(AssetClass);
	}

	return false;
}

void UThinkGraphSchema::CreateEdgeConnection(UEdGraphPin* PinA, UEdGraphPin* PinB, const UThinkGraphEdNode* OwningNodeA,
                                             const UThinkGraphEdNode* OwningNodeB) const
{
	const FVector2D InitPos((OwningNodeA->NodePosX + OwningNodeB->NodePosX) / 2,
	                        (OwningNodeA->NodePosY + OwningNodeB->NodePosY) / 2);

	// UInputAction* DefaultInput = nullptr;
	// if (const UThinkGraphEdGraph* HBThinkGraph = Cast<UThinkGraphEdGraph>(OwningNodeA->GetGraph()))
	// {
	// 	if (const UHBThinkGraph* HBThinkGraphModel = HBThinkGraph->GetThinkGraphModel())
	// 	{
	// 		DefaultInput = HBThinkGraphModel->DefaultInputAction;
	// 	}
	// }

	FThinkGraphSchemaAction_NewEdge Action;
	Action.NodeTemplate = NewObject<UThinkGraphEdNodeEdge>(OwningNodeA->GetGraph());

	UThinkGraphEdge* RuntimeEdge = NewObject<UThinkGraphEdge>(Action.NodeTemplate, UThinkGraphEdge::StaticClass());
	// RuntimeEdge->TransitionInput = DefaultInput;
	Action.NodeTemplate->SetRuntimeEdge(RuntimeEdge);

	Action.PerformAction(OwningNodeA->GetGraph(), PinB, InitPos, true);
	// Always create connections from node A to B, don't allow adding in reverse
	Action.NodeTemplate->CreateConnections(OwningNodeA, OwningNodeB);
}

void UThinkGraphSchema::CreateAndAddActionToContextMenu(FGraphContextMenuBuilder& ContextMenuBuilder,
                                                        const TSubclassOf<UTGNode> NodeType) const
{
	if (!NodeType)
	{
		return;
	}


	UTGNode* NodeCDO = NodeType.GetDefaultObject();
	if (!NodeCDO)
	{
		return;
	}

	const FText AddToolTip = LOCTEXT("NewThinkGraphNodeTooltip", "Add node here");

	FText Description = NodeCDO->ContextMenuName;
	FString NodeName = NodeType->GetName();
	NodeName.RemoveFromEnd("_C");

	// Shouldn't happen. ContextMenuName if empty will prevent execution of CreateAndAddActionToContextMenu
	if (Description.IsEmpty())
	{
		Description = FText::FromString(NodeName);
	}

	// If BP node type, set category to something specific (can be further defined if Context Menu Name, with a "|" which will override the category)
	FText Category = NodeType->IsNative()
		                 ? LOCTEXT("ThinkGraphNodeAction", "Think Graph ")
		                 : FText::FromString(TEXT("Custom"));

	// Split up ContextMenuName by "|" and create top category if there is more than one level
	const FString ContextMenu = NodeCDO->ContextMenuName.ToString();
	FString Left, Right;
	if (ContextMenu.Split(TEXT("|"), &Left, &Right))
	{
		Category = FText::FromString(Left.TrimStartAndEnd());
		Description = FText::FromString(Right.TrimStartAndEnd());
	}

	// If BP node, append class name
	if (!NodeType->IsNative() && NodeCDO->bIncludeClassNameInContextMenu)
	{
		Description = FText::FromString(FString::Printf(TEXT("%s (%s)"), *Description.ToString(), *NodeName));
	}

	const TSharedPtr<FThinkGraphSchemaAction_NewNode> Action(
		new FThinkGraphSchemaAction_NewNode(Category, Description, AddToolTip, 1));
	Action->NodeTemplate = NewObject<UThinkGraphEdNode>(ContextMenuBuilder.OwnerOfTemporaries, "Template");
	Action->NodeTemplate->RuntimeNode = NewObject<UTGNode>(Action->NodeTemplate, NodeType);
	ContextMenuBuilder.AddAction(Action);
}

bool UThinkGraphSchema::IsHoverPinMatching(const UEdGraphPin* InHoverPin)
{
	check(InHoverPin);

	const bool bIsValidName = InHoverPin->PinName == UThinkGraphPinNames::PinName_In ||
		InHoverPin->PinName == UThinkGraphPinNames::PinName_Out;

	return (InHoverPin->Direction == EGPD_Input || InHoverPin->Direction == EGPD_Output) && bIsValidName;
}

FVector2D UThinkGraphSchema::GetFixedOffsetFromPin(const UEdGraphPin* InHoverPin)
{
	// If don't have access to bounding information for node, using fixed offset that should work for most cases.
	// Start at 450.f, and is further refined based on SlateNode bounding info is available
	FVector2D FixedOffset(450.0f, 0.f);
	FVector2D DesiredSize(0.f, 0.f);

	if (const UThinkGraphEdNode* EdGraphNode = Cast<UThinkGraphEdNode>(InHoverPin->GetOwningNode()))
	{
		if (EdGraphNode->SlateNode)
		{
			DesiredSize = EdGraphNode->SlateNode->ComputeDesiredSize(FSlateApplication::Get().GetApplicationScale());
		}
	}

	// if (InHoverPin->PinName == UThinkGraphPinNames::PinName_In)
	// {
	// 	FixedOffset = FVector2D(-(DesiredSize.X * 2 + 100.f), -DesiredSize.Y);
	// }
	// else if (InHoverPin->PinName == UThinkGraphPinNames::PinName_In)
	// {
	// 	FixedOffset = FVector2D(-DesiredSize.X, -(DesiredSize.Y * 2 + 100.f));
	// }
	// else if (InHoverPin->PinName == UThinkGraphPinNames::PinName_Out)
	// {
	// 	FixedOffset = FVector2D(100.f, -DesiredSize.Y);
	// }
	// else if (InHoverPin->PinName == UThinkGraphPinNames::PinName_Out)
	// {
	// 	FixedOffset = FVector2D(-DesiredSize.X, 100.f);
	// }


	return DesiredSize + FixedOffset;
}

#undef LOCTEXT_NAMESPACE
