#include "ThinkGraphEdGraph.h"


#include "..\ThinkGraphEditorLog.h"
#include "SGraphNode.h"
#include "SGraphPanel.h"
#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/ThinkGraphEdge.h"
#include "Nodes/ThinkGraphEdNode.h"

#include "Nodes/ThinkGraphEdNodeEdge.h"
#include "Nodes\ThinkGraphEdNode_LLM.h"
#include "ThinkGraph/Nodes/ThinkGraphNode.h"

UThinkGraph* UThinkGraphEdGraph::GetThinkGraphModel() const
{
	return CastChecked<UThinkGraph>(GetOuter());
}

void UThinkGraphEdGraph::RebuildGraph()
{
	MG_ERROR(Verbose, TEXT("UThinkGraphEdGraph::RebuildGraph has been called. Nodes Num: %d"), Nodes.Num())

	UThinkGraph* ThinkGraph = GetThinkGraphModel();
	check(ThinkGraph)

	Clear();

	for (UEdGraphNode* CurrentNode : Nodes)
	{
		MG_ERROR(Verbose, TEXT("UThinkGraphEdGraph::RebuildGraph for node: %s (%s)"), *CurrentNode->GetName(),
		         *CurrentNode->GetClass()->GetName())

		if (UThinkGraphEdNode_LLM* GraphEntryNode = Cast<UThinkGraphEdNode_LLM>(CurrentNode))
		{
			RebuildGraphForEntry(ThinkGraph, GraphEntryNode);
		}
		else if (UThinkGraphEdNodeEdge* GraphEdge = Cast<UThinkGraphEdNodeEdge>(CurrentNode))
		{
			RebuildGraphForEdge(ThinkGraph, GraphEdge);
		}
		else if (UThinkGraphEdNode* Node = Cast<UThinkGraphEdNode>(CurrentNode))
		{
			RebuildGraphForNode(ThinkGraph, Node);
		}

	}


	for (UThinkGraphNode* Node : ThinkGraph->AllNodes)
	{
		if (Node->ParentNodes.Num() == 0)
		{
			// ThinkGraph->RootNodes.Add(Node);
			// May cause a weird issue, no crash but editor goes unresponsive
			// SortNodes(Node);
		}

		Node->Rename(nullptr, ThinkGraph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void UThinkGraphEdGraph::RebuildGraphForEdge(UThinkGraph* OwningGraph, UThinkGraphEdNodeEdge* EdGraphEdge)
{
	UThinkGraphEdNode* StartNode = EdGraphEdge->GetStartNodeAsBase();
	UThinkGraphEdNode* EndNode = EdGraphEdge->GetEndNode();
	UThinkGraphEdge* Edge = EdGraphEdge->RuntimeEdge;

	if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
	{
		MG_ERROR(
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

void UThinkGraphEdGraph::RebuildGraphForNode(UThinkGraph* OwningGraph, UThinkGraphEdNode* NodeBase)
{
	UThinkGraphEdNode* Node = Cast<UThinkGraphEdNode>(NodeBase);
	UThinkGraphNode* RuntimeNode = Node
		                                 ? Node->RuntimeNode
		                                 : nullptr;
	if (!RuntimeNode)
	{
		return;
	}

	NodeMap.Add(RuntimeNode, NodeBase);

	OwningGraph->AllNodes.Add(RuntimeNode);
	AnimIndex++;

	Node->NodeIndex = AnimIndex;

	for (UEdGraphPin* Pin : NodeBase->Pins)
	{
		if (Pin->Direction != EGPD_Output)
		{
			continue;
		}

		for (const UEdGraphPin* LinkedTo : Pin->LinkedTo)
		{
			UThinkGraphNode* ChildNode = nullptr;

			// Try to determine child node
			if (const UThinkGraphEdNode* OwningNode = Cast<UThinkGraphEdNode>(
				LinkedTo->GetOwningNode()))
			{
				ChildNode = OwningNode->RuntimeNode;
			}
			else if (const UThinkGraphEdNodeEdge* OwningEdge = Cast<UThinkGraphEdNodeEdge>(
				LinkedTo->GetOwningNode()))
			{
				UThinkGraphEdNode* EndNode = OwningEdge->GetEndNode();
				if (EndNode)
				{
					ChildNode = EndNode->RuntimeNode;
				}
			}

			// Update child / parent nodes for both node and containing ability graph
			if (ChildNode)
			{
				RuntimeNode->ChildrenNodes.Add(ChildNode);
				ChildNode->ParentNodes.Add(RuntimeNode);
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UThinkGraphEdGraph::RebuildGraphForEntry(UThinkGraph* OwningGraph, UThinkGraphEdNode_LLM* NodeEntry)
{
	check(NodeEntry);
	check(OwningGraph);

	UThinkGraphEdNode* ConnectedToNode = Cast<UThinkGraphEdNode>(NodeEntry->GetOutputNode());

	MG_ERROR(Verbose, TEXT("UThinkGraphEdGraph::RebuildGraphForEntry ... Node: %s"),
	         ConnectedToNode ? *ConnectedToNode->GetName() : TEXT("NONE"))

	if (ConnectedToNode)
	{
		NodeEntry->RuntimeNode->ChildrenNodes.Add(ConnectedToNode->RuntimeNode);
		ConnectedToNode->RuntimeNode->ParentNodes.Add(NodeEntry->RuntimeNode);
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
	TArray<UThinkGraphEdNode*> OutNodes;
	for (UEdGraphNode* EdNode : Nodes)
	{
		if (UThinkGraphEdNode* Node = Cast<UThinkGraphEdNode>(EdNode))
		{
			OutNodes.Add(Node);
		}
	}

	return OutNodes;
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

	void AutoArrangeNodesVertically(UThinkGraphEdNode* ParentNode, FNodeBoundsInfo& BBoxTree, float PosX, float PosY)
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
							BBoxTree.Children[BBoxIndex].SubGraphBBox.X / 2 - GraphNode->DEPRECATED_NodeWidget.Pin()->
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
							PosX, BBoxTree.Children[BBoxIndex].SubGraphBBox.Y / 2 - GraphNode->DEPRECATED_NodeWidget.
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

	MG_ERROR(Verbose, TEXT("UThinkGraphEdGraph::AutoArrange Strategy: %s"),
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
		                                                  RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().Y *
		                                                  2.5f);

		const float NewRootPosX = BBoxTree.SubGraphBBox.X / 2 - RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().
		                                                                  X / 2;
		RootNode->DEPRECATED_NodeWidget.Pin()->MoveTo(FVector2D(NewRootPosX, 0), NodeFilter);
	}
	else
	{
		ACEAutoArrangeHelpers::AutoArrangeNodesHorizontally(RootNode, BBoxTree,
		                                                    RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().X *
		                                                    2.5f, 0);

		const float NewRootPosY = BBoxTree.SubGraphBBox.Y / 2 - RootNode->DEPRECATED_NodeWidget.Pin()->GetDesiredSize().
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
	}

	NodeMap.Reset();
	EdgeMap.Reset();
	EntryNodes.Reset();

	for (UEdGraphNode* Node : Nodes)
	{
		if (const UThinkGraphEdNode* EdGraphNode = Cast<UThinkGraphEdNode>(Node))
		{
			UThinkGraphNode* GraphNode = EdGraphNode->RuntimeNode;
			if (GraphNode)
			{
				GraphNode->ParentNodes.Reset();
				GraphNode->ChildrenNodes.Reset();
			}
		}
	}
}

void UThinkGraphEdGraph::SortNodes(UThinkGraphNode* RootNode)
{
	TArray<UThinkGraphNode*> CurrLevelNodes = {RootNode};
	TArray<UThinkGraphNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (UThinkGraphNode* Node : CurrLevelNodes)
		{
			auto Comp = [&](const UThinkGraphNode& L, const UThinkGraphNode& R)
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
