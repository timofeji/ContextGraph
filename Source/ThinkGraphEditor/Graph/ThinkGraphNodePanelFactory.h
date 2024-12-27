// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

/**
 * 
 */
struct FThinkGraphNodePanelFactory : public FGraphPanelNodeFactory
{
public:
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};


