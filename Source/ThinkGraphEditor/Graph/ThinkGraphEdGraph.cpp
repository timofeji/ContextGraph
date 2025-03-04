#include "ThinkGraphEdGraph.h"


#include "ClearReplacementShaders.h"
#include "..\ThinkGraphEditorLog.h"
#include "SGraphNode.h"
#include "SGraphPanel.h"
#include "ThinkGraphEditorTypes.h"
#include "ThinkGraphSchema.h"
#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/ThinkGraphEdge.h"
#include "Nodes/ThinkGraphEdNode.h"

#include "Nodes/ThinkGraphEdNodeEdge.h"
#include "Nodes\ThinkGraphEdNode_LLM.h"
#include "Nodes\ThinkGraphEdNode_Memory.h"
#include "..\..\ThinkGraph\Nodes\TGNode.h"
#include "Nodes/ThinkGraphEdNode_Const.h"
#include "Nodes/ThinkGraphEdNode_Embed.h"
#include "Nodes/ThinkGraphEdNode_Stimulus.h"
#include "ThinkGraph/TGTypes.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Const.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Embed.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_LLM.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Memory.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Stimulus.h"

UThinkGraph* UThinkGraphEdGraph::GetThinkGraphModel() const
{
	return CastChecked<UThinkGraph>(GetOuter());
}

void UThinkGraphEdGraph::RebuildGraph()
{
	TGE_ERROR(Verbose, TEXT("UThinkGraphEdGraph::RebuildGraph has been called. Nodes Num: %d"), Nodes.Num())

	UThinkGraph* ThinkGraph = GetThinkGraphModel();
	check(ThinkGraph)

	Clear();
	Modify(true);


	//* REBUILD GRAPH TOPOLOGY*//
	// Backwards pass
	for (UEdGraphNode* CurrentNode : Nodes)
	{
		UE_LOG(LogThinkGraphEditor, Verbose, TEXT("UThinkGraphEdGraph::RebuildGraph for node: %s (%s)"),
		       *CurrentNode->GetName(),
		       *CurrentNode->GetClass()->GetName())

		if (UThinkGraphEdNode_Memory* MemoryNode = Cast<UThinkGraphEdNode_Memory>(CurrentNode))
		{
			RebuildGraphForNode(ThinkGraph, MemoryNode);
		}
	}

	//* POPULATE BUFFERS WITH INITIAL VALUES*//
	// we traverse all nodes in reverse order since the leaf nodes should be processed first
	// Forward pass
	for (int i = ThinkGraph->AllNodes.Num() - 1; i >= 0; i--)
	{
		auto Node = ThinkGraph->AllNodes[i];


		if (auto MemoryNode = Cast<UThinkGraphNode_Memory>(Node))
		{
			if (!MemoryNode->InBufferIDS.IsEmpty())
			{
				FString Key = MemoryNode->GetNodeTitle().ToString();
				ThinkGraph->OutBuffers.Add(
					FName(Key.IsEmpty() ? "mem" : Key), MemoryNode->InBufferIDS[0]);
			}
		}

		if (auto StimulusNode = Cast<UThinkGraphNode_Stimulus>(Node))
		{
			ThinkGraph->InBuffers.Add(
				FName(StimulusNode->GetNodeTitle().ToString()),
				StimulusNode->OutBufferIDS[0]);
		}

		if (auto constNode = Cast<UThinkGraphNode_Const>(Node))
		{
			for (const uint16 OutBufferID : Node->OutBufferIDS)
			{
				ThinkGraph->GetBuffer(OutBufferID);
			}
		}
	}
}

void UThinkGraphEdGraph::RebuildGraphForEdge(UThinkGraph* OwningGraph, UThinkGraphEdNodeEdge* EdGraphEdge)
{
	UThinkGraphEdNode* StartNode = EdGraphEdge->GetStartNodeAsBase();
	UThinkGraphEdNode* EndNode = EdGraphEdge->GetEndNode();
	UThinkGraphEdge* Edge = EdGraphEdge->RuntimeEdge;

	if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
	{
		TGE_ERROR(
			Error,
			TEXT("UThinkGraphEdGraph::RebuildGraph add edge failed. StartNode: %s, EndNode: %s, Edge: %s"),
			StartNode ? *StartNode->GetName() : TEXT("NONE"),
			EndNode ? *EndNode->GetName() : TEXT("NONE"),
			Edge ? *Edge->GetName() : TEXT("NONE")
		);
		return;
	}

	EdGraphEdge->UpdateCachedIcon();

	EdgeMap.Add(Edge, EdGraphEdge);

	Edge->Rename(nullptr, OwningGraph, REN_DontCreateRedirectors | REN_DoNotDirty);

	if (const UThinkGraphEdNode* Node{Cast<UThinkGraphEdNode>(StartNode)})
	{
		Edge->StartNode = Node->RuntimeNode;
	}

	Edge->EndNode = EndNode->RuntimeNode;
}


void UThinkGraphEdGraph::RebuildGraphForEmbed(UThinkGraph* ThinkGraph, UThinkGraphEdNode_Embed* EmbedEdNode)
{
	if (auto EmbedNode = Cast<UThinkGraphNode_Embed>(EmbedEdNode->RuntimeNode))
	{
		EmbedNode->VarKeys.Empty();
		for (auto Pin : EmbedEdNode->Pins)
		{
			if (Pin->PinType.PinCategory == UThinkGraphPinNames::PinCategory_Value &&
				!Pin->PinType.bIsConst)
			{
				EmbedNode->VarKeys.Add(Pin->GetName());
			}
		}
	}

	NotifyNodeChanged(EmbedEdNode);
}


void UThinkGraphEdGraph::PopulateBuffersForConst(UThinkGraph* OwningGraph, UThinkGraphEdNode_Const* ConstEdNode)
{
	if (ConstEdNode->RuntimeNode->OutBufferIDS.Num())
	{
		FDataBuffer* Buffer;
		Buffer = &OwningGraph->GetBuffer(ConstEdNode->RuntimeNode->OutBufferIDS[0]);
		Buffer->Update(FText::FromString(ConstEdNode->Text));
	}
}

void UThinkGraphEdGraph::RebuildGraphForNode(UThinkGraph* OwningGraph, UThinkGraphEdNode* EdNode)
{
	if (!EdNode->Pins.Num())
	{
		return;
	}

	if (auto EmbedEdNode = Cast<UThinkGraphEdNode_Embed>(EdNode))
	{
		RebuildGraphForEmbed(OwningGraph, EmbedEdNode);
	}


	OwningGraph->AllNodes.Add(EdNode->RuntimeNode);
	EdNode->Modify();


	for (auto Pin : EdNode->Pins)
	{
		if (Pin->Direction == EGPD_Output)
		{
			FDataBuffer* Buffer;
			if (EdNode->RuntimeNode->OutBufferIDS.IsEmpty())
			{
				Buffer = &OwningGraph->AddDataBuffer();
				EdNode->RuntimeNode->OutBufferIDS.Add(Buffer->BufferID);
			}
			else
			{
				Buffer = &OwningGraph->GetBuffer(EdNode->RuntimeNode->OutBufferIDS[0]);
			}

			Buffer->NodeDependancies.AddUnique(EdNode->RuntimeNode);

			if (auto ConstEdNode = Cast<UThinkGraphEdNode_Const>(EdNode))
			{
				PopulateBuffersForConst(OwningGraph, ConstEdNode);
			}


			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto LinkedToEdNode = Cast<UThinkGraphEdNode>(LinkedTo->GetOwningNode()))
				{
					LinkedToEdNode->RuntimeNode->InBufferIDS.AddUnique(Buffer->BufferID);

					Buffer->NodeDependancies.AddUnique(LinkedToEdNode->RuntimeNode);

					if (auto EmbedEdNode = Cast<UThinkGraphEdNode_Embed>(LinkedToEdNode))
					{
						EmbedEdNode->OnTemplateUpdated();
					}
				}
			}
		}
		else if (Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() > 0)
		{
			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto LinkedToEdNode = Cast<UThinkGraphEdNode>(LinkedTo->GetOwningNode()))
				{
					RebuildGraphForNode(OwningGraph, LinkedToEdNode);
				}
			}
		}
	}
}


void UThinkGraphEdGraph::ValidateNodes(FCompilerResultsLog* LogResults)
{
	for (const UEdGraphNode* Node : Nodes)
	{
		if (const UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(Node))
		{
			Node->ValidateNodeDuringCompilation(*LogResults);
		}
	}
}

bool UThinkGraphEdGraph::Modify(const bool bAlwaysMarkDirty)
{
	const bool bWasSaved = Super::Modify(bAlwaysMarkDirty);

	UThinkGraph* ThinkGraphModel = GetThinkGraphModel();
	if (ThinkGraphModel)
	{
		ThinkGraphModel->Modify();
	}

	for (UEdGraphNode* Node : Nodes)
	{
		Node->Modify();
	}

	return bWasSaved;
}

void UThinkGraphEdGraph::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}

TArray<UThinkGraphEdNode*> UThinkGraphEdGraph::GetAllNodes() const
{
	TArray<UThinkGraphEdNode*> OutBuffers;
	for (UEdGraphNode* EdNode : Nodes)
	{
		if (UThinkGraphEdNode* Node = Cast<UThinkGraphEdNode>(EdNode))
		{
			OutBuffers.Add(Node);
		}
	}

	return OutBuffers;
}

namespace ACEAutoArrangeHelpers
{
	struct FNodeBoundsInfo
	{
		FVector2D SubGraphBBox;
		TArray<FNodeBoundsInfo> Children;
	};

	UEdGraphPin* FindGraphNodePin(UEdGraphNode* Node, const EEdGraphPinDirection Dir)
	{
		UEdGraphPin* Pin = nullptr;
		for (int32 Idx = 0; Idx < Node->Pins.Num(); Idx++)
		{
			if (Node->Pins[Idx]->Direction == Dir)
			{
				Pin = Node->Pins[Idx];
				break;
			}
		}

		return Pin;
	}

	void AutoArrangeNodesVertically(UThinkGraphEdNode* ParentNode, FNodeBoundsInfo& BBoxTree, float PosX,
	                                float PosY)
	{
		int32 BBoxIndex = 0;

		UEdGraphPin* Pin = FindGraphNodePin(ParentNode, EGPD_Output);
		if (Pin)
		{
			SGraphNode::FNodeSet NodeFilter;
			TArray<UEdGraphPin*> TempLinkedTo = Pin->LinkedTo;
			for (int32 Idx = 0; Idx < TempLinkedTo.Num(); Idx++)
			{
				UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(TempLinkedTo[Idx]->GetOwningNode());
				if (GraphNode && BBoxTree.Children.Num() > 0)
				{
					AutoArrangeNodesVertically(GraphNode, BBoxTree.Children[BBoxIndex], PosX,
					                           PosY + GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y *
					                           2.5f);
					GraphNode->DEPRECATED_NodeWidget.Pin()->MoveTo(
						FVector2D(
							BBoxTree.Children[BBoxIndex].SubGraphBBox.X / 2 - GraphNode->DEPRECATED_NodeWidget.Pin()
							->
							GetDesiredSize().X / 2 + PosX, PosY), NodeFilter);
					PosX += BBoxTree.Children[BBoxIndex].SubGraphBBox.X + 20;
					BBoxIndex++;
				}
			}
		}
	}

	void AutoArrangeNodesHorizontally(UThinkGraphEdNode* ParentNode, FNodeBoundsInfo& BBoxTree, float PosX,
	                                  float PosY)
	{
		int32 BBoxIndex = 0;

		UEdGraphPin* Pin = FindGraphNodePin(ParentNode, EGPD_Output);
		if (Pin)
		{
			SGraphNode::FNodeSet NodeFilter;
			TArray<UEdGraphPin*> TempLinkedTo = Pin->LinkedTo;
			for (int32 Idx = 0; Idx < TempLinkedTo.Num(); Idx++)
			{
				UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(TempLinkedTo[Idx]->GetOwningNode());
				if (GraphNode && BBoxTree.Children.Num() > 0)
				{
					// AutoArrangeNodesVertically(GraphNode, BBoxTree.Children[BBoxIndex], PosX, PosY + GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y * 2.5f);
					AutoArrangeNodesHorizontally(GraphNode, BBoxTree.Children[BBoxIndex],
					                             PosX + GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X *
					                             2.5f, PosY);

					// GraphNode->DEPRECATED_NodeWidget.Pin()->MoveTo(FVector2D(BBoxTree.Children[BBoxIndex].SubGraphBBox.X / 2 - GraphNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X / 2 + PosX, PosY), NodeFilter);
					GraphNode->DEPRECATED_NodeWidget.Pin()->MoveTo(
						FVector2D(
							PosX, BBoxTree.Children[BBoxIndex].SubGraphBBox.Y / 2 - GraphNode->DEPRECATED_NodeWidget
							.
							Pin()->GetDesiredSize().Y / 2 + PosY), NodeFilter);

					// PosX += BBoxTree.Children[BBoxIndex].SubGraphBBox.X + 20;
					PosY += BBoxTree.Children[BBoxIndex].SubGraphBBox.Y + 20;
					BBoxIndex++;
				}
			}
		}
	}

	void GetNodeSizeInfo(UThinkGraphEdNode* ParentNode, FNodeBoundsInfo& BBoxTree,
	                     TArray<UThinkGraphEdNode*>& VisitedNodes)
	{
		BBoxTree.SubGraphBBox = ParentNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize();
		float LevelWidth = 0;
		float LevelHeight = 0;

		UEdGraphPin* Pin = FindGraphNodePin(ParentNode, EGPD_Output);
		if (Pin)
		{
			// Pin->LinkedTo.Sort(FThinkGraphCompareNodeXLocation());
			for (int32 Idx = 0; Idx < Pin->LinkedTo.Num(); Idx++)
			{
				UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(Pin->LinkedTo[Idx]->GetOwningNode());
				if (GraphNode)
				{
					const int32 ChildIdx = BBoxTree.Children.Add(FNodeBoundsInfo());
					FNodeBoundsInfo& ChildBounds = BBoxTree.Children[ChildIdx];

					if (!VisitedNodes.Contains(GraphNode))
					{
						VisitedNodes.AddUnique(GraphNode);
						GetNodeSizeInfo(GraphNode, ChildBounds, VisitedNodes);
					}

					LevelWidth += ChildBounds.SubGraphBBox.X + 20;
					if (ChildBounds.SubGraphBBox.Y > LevelHeight)
					{
						LevelHeight = ChildBounds.SubGraphBBox.Y;
					}
				}
			}

			if (LevelWidth > BBoxTree.SubGraphBBox.X)
			{
				BBoxTree.SubGraphBBox.X = LevelWidth;
			}

			BBoxTree.SubGraphBBox.Y += LevelHeight;
		}
	}
}


void UThinkGraphEdGraph::AutoArrange(const bool bVertical)
{
	UThinkGraphEdNode* RootNode = nullptr;
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		RootNode = Cast<UThinkGraphEdNode_LLM>(Nodes[Idx]);
		if (RootNode)
		{
			break;
		}
	}

	if (!RootNode)
	{
		return;
	}

	TGE_ERROR(Verbose, TEXT("UThinkGraphEdGraph::AutoArrange Strategy: %s"),
	          bVertical ? TEXT("Vertical") : TEXT("Horizontal"))
	const FScopedTransaction Transaction(NSLOCTEXT("ACEGraph", "ThinkGraphEditorAutoArrange",
	                                               "Think Graph Editor: Auto Arrange"));

	ACEAutoArrangeHelpers::FNodeBoundsInfo BBoxTree;
	TArray<UThinkGraphEdNode*> VisitedNodes;
	ACEAutoArrangeHelpers::GetNodeSizeInfo(RootNode, BBoxTree, VisitedNodes);

	SGraphNode::FNodeSet NodeFilter;
	if (bVertical)
	{
		ACEAutoArrangeHelpers::AutoArrangeNodesVertically(RootNode, BBoxTree, 0,
		                                                  RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y
		                                                  *
		                                                  2.5f);

		const float NewRootPosX = BBoxTree.SubGraphBBox.X / 2 - RootNode->
		                                                        DEPRECATED_NodeWidget.Pin()->GetDesiredSize().
		                                                        X / 2;
		RootNode->DEPRECATED_NodeWidget.Pin()->MoveTo(FVector2D(NewRootPosX, 0), NodeFilter);
	}
	else
	{
		ACEAutoArrangeHelpers::AutoArrangeNodesHorizontally(RootNode, BBoxTree,
		                                                    RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().
		                                                              X *
		                                                    2.5f, 0);

		const float NewRootPosY = BBoxTree.SubGraphBBox.Y / 2 - RootNode->
		                                                        DEPRECATED_NodeWidget.Pin()->GetDesiredSize().
		                                                        Y / 2;
		RootNode->DEPRECATED_NodeWidget.Pin()->MoveTo(FVector2D(0, NewRootPosY), NodeFilter);
	}

	RootNode->DEPRECATED_NodeWidget.Pin()->GetOwnerPanel()->ZoomToFit(/*bOnlySelection=*/ false);
}

void UThinkGraphEdGraph::Clear()
{
	AnimIndex = 0;

	UThinkGraph* ThinkGraphModel = GetThinkGraphModel();
	if (ThinkGraphModel)
	{
		ThinkGraphModel->ClearGraph();
		ThinkGraphModel->Modify(true);
	}

	NodeMap.Reset();
	EdgeMap.Reset();
	EntryNodes.Reset();


	for (UEdGraphNode* Node : Nodes)
	{
		if (const UThinkGraphEdNode* EdGraphNode = Cast<UThinkGraphEdNode>(Node))
		{
			UTGNode* GraphNode = EdGraphNode->RuntimeNode;
			if (GraphNode)
			{
				GraphNode->InBufferIDS.Reset();
				GraphNode->OutBufferIDS.Reset();
			}
		}
	}
}

void UThinkGraphEdGraph::SortNodes(UTGNode* RootNode)
{
	TArray<UTGNode*> CurrLevelNodes = {RootNode};
	TArray<UTGNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (UTGNode* Node : CurrLevelNodes)
		{
			auto Comp = [&](const UTGNode& L, const UTGNode& R)
			{
				const UThinkGraphEdNode* EdNode_LNode = NodeMap[&L];
				const UThinkGraphEdNode* EdNode_RNode = NodeMap[&R];
				return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
			};

			Node->ChildrenNodes.Sort(Comp);
			Node->ParentNodes.Sort(Comp);

			NextLevelNodes.Append(Node->ChildrenNodes);
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
	}
}
