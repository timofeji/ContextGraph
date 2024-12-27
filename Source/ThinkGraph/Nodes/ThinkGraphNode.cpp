// Created by Timofej Jermolaev, All rights reserved . 


#include "ThinkGraphNode.h"


#define LOCTEXT_NAMESPACE "ThinkGraphNode"

FText UThinkGraphNode::GetNodeTitle() const
{
	FText DefaultText = LOCTEXT("DefaultNodeTitle", "Think Graph Node");
	return NodeTitle.IsEmpty() ? DefaultText : NodeTitle;
}

// UThinkGraphEdge* UThinkGraphNode::GetEdge(UThinkGraphNode* ChildNode)
// {
// 	return Edges.Contains(ChildNode) ? Edges.FindChecked(ChildNode) : nullptr;
// }

bool UThinkGraphNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}


bool UThinkGraphNode::IsHBActionSubclassedInBlueprint() const
{
	UClass* NodeClass = GetClass();
	if (!NodeClass)
	{
		return false;
	}

	return !NodeClass->IsNative();
}
#if WITH_EDITOR
bool UThinkGraphNode::IsNameEditable() const
{
	return true;
}

void UThinkGraphNode::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}

bool UThinkGraphNode::CanCreateConnection(UThinkGraphNode* Other, FText& ErrorMessage)
{
	return true;
}

bool UThinkGraphNode::CanCreateConnectionTo(UThinkGraphNode* Other, const int32 NumberOfChildrenNodes, FText& ErrorMessage)
{
	// if (ChildrenLimitType == EACENodeLimit::Limited && NumberOfChildrenNodes >= ChildrenLimit)
	// {
	// 	ErrorMessage = FText::FromString("Children limit exceeded");
	// 	return false;
	// }

	return CanCreateConnection(Other, ErrorMessage);
}

bool UThinkGraphNode::CanCreateConnectionFrom(UThinkGraphNode* Other, const int32 NumberOfParentNodes, FText& ErrorMessage)
{
	return CanCreateConnection(Other, ErrorMessage);
}

#endif

#undef LOCTEXT_NAMESPACE
