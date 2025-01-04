// Created by Timofej Jermolaev, All rights reserved . 


#include "ThinkGraphNodePanelFactory.h"

// #include "ThinkGraphEditorSettings.h"
#include "ThinkGraphEditorSettings.h"
#include "Nodes/ThinkGraphEdNode_Const.h"
#include "Nodes/ThinkGraphEdNode_Embed.h"
#include "Nodes/ThinkGraphEdNode_Parse.h"
#include "Slate/SThinkGraphNode_Const.h"
#include "Slate/SThinkGraphNode.h"
#include "Slate/SThinkGraphNode_Embed.h"


TSharedPtr<SGraphNode> FThinkGraphNodePanelFactory::CreateNode(UEdGraphNode* Node) const
{

	if (UThinkGraphEdNode_Embed* GraphEdge = Cast<UThinkGraphEdNode_Embed>(Node))
	{
		return SNew(SThinkGraphNode_Embed, GraphEdge);
	}
	
	if (UThinkGraphEdNode_Const* PromptNode = Cast<UThinkGraphEdNode_Const>(Node))
	{
		return SNew(SThinkGraphNode_Const, PromptNode);
	}
	
	if (UThinkGraphEdNode* GraphNode = Cast<UThinkGraphEdNode>(Node))
	{
		const UThinkGraphEditorSettings* Settings = GetDefault<UThinkGraphEditorSettings>();
	
		// const bool bDrawVerticalPins = Settings->DrawPinTypes == EComboGraphDrawPinTypes::Both || Settings->DrawPinTypes == EComboGraphDrawPinTypes::Vertical;
		// const bool bDrawHorizontalPins = Settings->DrawPinTypes == EComboGraphDrawPinTypes::Both || Settings->DrawPinTypes == EComboGraphDrawPinTypes::Horizontal;
	
		return SNew(SThinkGraphNode, GraphNode)
			.PinSize(Settings->PinSize)
			.PinPadding(Settings->PinPadding)
			.DrawVerticalPins(true)
			.DrawHorizontalPins(true)
			.ContentPadding(Settings->ContentMargin)
			.ContentInternalPadding(Settings->ContentInternalPadding);
	}

	return nullptr;
}
