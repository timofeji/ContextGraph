// Created by Timofej Jermolaev, All rights reserved . 


#include "ThinkGraphNodePanelFactory.h"

// #include "ThinkGraphEditorSettings.h"
#include "ThinkGraphEditorSettings.h"
#include "Nodes/ThinkGraphEdNode_BasePrompt.h"
#include "Nodes/ThinkGraphEdNode_Parse.h"
#include "Slate/SThinkGraphNode_Prompt.h"
#include "Slate/SThinkGraphNode.h"


TSharedPtr<SGraphNode> FThinkGraphNodePanelFactory::CreateNode(UEdGraphNode* Node) const
{

	// if (UThinkGraphEdNodeEdge* GraphEdge = Cast<UThinkGraphEdNodeEdge>(Node))
	// {
	// 	return SNew(SThinkGraphEdge, GraphEdge);
	// }
	//
	// if (UThinkGraphEdNode_LLM* EntryNode = Cast<UThinkGraphEdNode_LLM>(Node))
	// {
	// 	return SNew(SThinkGraphNode_Prompt, EntryNode);
	// }
	
	if (UThinkGraphEdNode_BasePrompt* PromptNode = Cast<UThinkGraphEdNode_BasePrompt>(Node))
	{
		return SNew(SThinkGraphNode_Prompt, PromptNode);
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
