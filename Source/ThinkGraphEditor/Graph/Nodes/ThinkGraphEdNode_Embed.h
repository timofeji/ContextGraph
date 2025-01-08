// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphEdNode.h"
#include "ThinkGraphEdNode_Embed.generated.h"

class UTGNode;
class UEdGraphPin;
/** Editor Graph Node for entry point in Think Graphs. Based off UAnimStateEntryNode for state machine graphs in Anim BP. */
UCLASS(MinimalAPI)
class UThinkGraphEdNode_Embed : public UThinkGraphEdNode
{
	GENERATED_BODY()

public:
	UThinkGraphEdNode_Embed();


	//~ Begin UEdGraphNode Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	bool ShowPaletteIconOnNode() const override;
	virtual FText GetTooltipText() const override;
	void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	

	virtual bool CanDuplicateNode() const override	{ return false; }
	//~ End UEdGraphNode Interface

	THINKGRAPHEDITOR_API UEdGraphNode* GetOutputNode();
	
	void ReallocateBindPins(TArray<FString>& Key);
	void OnTemplateUpdated();

	TSet<FString> ValueBindKeys;
	// // UPROPERTY()
	// // TArray<UEdGraphPin*> InputPins;
	//
	// TArray<UEdGraphPin*> GetInputPins(){return InputPins;};

};
