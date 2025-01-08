// Created by Timofej Jermolaev, All rights reserved . 


#include "ThinkGraphNodePanelFactory.h"

// #include "ThinkGraphEditorSettings.h"
#include "ThinkGraphEditorSettings.h"
#include "Nodes/ThinkGraphEdNode_Const.h"
#include "Nodes/ThinkGraphEdNode_Embed.h"
#include "Nodes/ThinkGraphEdNode_Memory.h"
#include "Nodes/ThinkGraphEdNode_Stimulus.h"
#include "Slate/SThinkGraphNode_Value.h"
#include "Slate/SThinkGraphNode.h"
#include "Slate/SThinkGraphNode_Embed.h"


TSharedPtr<SGraphNode> FThinkGraphNodePanelFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UThinkGraphEdNode_Embed* EmbedNode = Cast<UThinkGraphEdNode_Embed>(Node))
	{
		return SNew(SThinkGraphNode_Embed, EmbedNode);
	}

	if (UThinkGraphEdNode_Const* PromptNode = Cast<UThinkGraphEdNode_Const>(Node))
	{
		return SNew(SThinkGraphNode_Value, PromptNode);
	}

	if (UThinkGraphEdNode_Memory* MemoryNode = Cast<UThinkGraphEdNode_Memory>(Node))
	{
		return SNew(SThinkGraphNode_Value, MemoryNode);
	}
	
	if (UThinkGraphEdNode_Stimulus* StimulusNode = Cast<UThinkGraphEdNode_Stimulus>(Node))
	{
		return SNew(SThinkGraphNode_Value, StimulusNode);
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
