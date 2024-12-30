// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphEdNode.h"
#include "ThinkGraphEdNode_Stimulus.generated.h"

class UThinkGraphNode;
/** Editor Graph Node for entry point in Think Graphs. Based off UAnimStateEntryNode for state machine graphs in Anim BP. */
UCLASS(MinimalAPI)
class UThinkGraphEdNode_Stimulus : public UThinkGraphEdNode
{
	GENERATED_BODY()

public:
	UThinkGraphEdNode_Stimulus();


	//~ Begin UEdGraphNode Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool ShowPaletteIconOnNode() const override;
	virtual FText GetTooltipText() const override;

	virtual bool CanDuplicateNode() const override	{ return false; }
	//~ End UEdGraphNode Interface

	THINKGRAPHEDITOR_API UEdGraphNode* GetOutputNode();
};
