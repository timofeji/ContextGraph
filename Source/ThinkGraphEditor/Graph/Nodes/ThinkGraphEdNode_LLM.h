// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphEdNode.h"
#include "ThinkGraphEdNode_LLM.generated.h"

class UTGNode;
/** Editor Graph Node for entry point in Think Graphs. Based off UAnimStateEntryNode for state machine graphs in Anim BP. */
UCLASS(MinimalAPI)
class UThinkGraphEdNode_LLM : public UThinkGraphEdNode
{
	GENERATED_BODY()

public:
	UThinkGraphEdNode_LLM();


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
