// Created by Timofej Jermolaev, All rights reserved . 


#include "TGNode.h"


#define LOCTEXT_NAMESPACE "ThinkGraphNode"

FText UTGNode::GetNodeTitle() const
{
	FText DefaultText = LOCTEXT("DefaultNodeTitle", "ThinkGraph Node");
	return NodeTitle.IsEmpty() ? DefaultText : NodeTitle;
}

// UThinkGraphEdge* UTGNode::GetEdge(UTGNode* ChildNode)
// {
// 	return Edges.Contains(ChildNode) ? Edges.FindChecked(ChildNode) : nullptr;
// }




bool UTGNode::IsHBActionSubclassedInBlueprint() const
{
	UClass* NodeClass = GetClass();
	if (!NodeClass)
	{
		return false;
	}

	return !NodeClass->IsNative();
}


#if WITH_EDITOR
bool UTGNode::IsNameEditable() const
{
	return true;
}

void UTGNode::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}

bool UTGNode::CanCreateConnection(UTGNode* Other, FText& ErrorMessage)
{
	return true;
}

bool UTGNode::CanCreateConnectionTo(UTGNode* Other, const int32 NumberOfChildrenNodes, FText& ErrorMessage)
{
	// if (ChildrenLimitType == EACENodeLimit::Limited && NumberOfChildrenNodes >= ChildrenLimit)
	// {
	// 	ErrorMessage = FText::FromString("Children limit exceeded");
	// 	return false;
	// }

	return CanCreateConnection(Other, ErrorMessage);
}

bool UTGNode::CanCreateConnectionFrom(UTGNode* Other, const int32 NumberOfParentNodes, FText& ErrorMessage)
{
	return CanCreateConnection(Other, ErrorMessage);
}

#endif

#undef LOCTEXT_NAMESPACE
