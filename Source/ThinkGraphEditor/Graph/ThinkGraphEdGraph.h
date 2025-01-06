
#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "Nodes/ThinkGraphEdNode_Embed.h"
#include "ThinkGraphEdGraph.generated.h"

class UThinkGraph;
class FThinkGraphDebugger;
class UThinkGraphEdNode;
class UThinkGraphEdNodeSelector;
class UTGNode;
class UThinkGraphNodeAnim;
class UThinkGraphEdge;
class UThinkGraphEdNodeEdge;
class UThinkGraphEdNode;
class UThinkGraphEdNode_LLM;
class UThinkGraphEdNode_Const;
class UThinkGraphEdNode_Memory;

UCLASS()
class THINKGRAPHEDITOR_API UThinkGraphEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	/** Entry node within the state machine */
	UPROPERTY()
	TArray<UThinkGraphEdNode*> EntryNodes;

	UPROPERTY(Transient)
	TMap<UTGNode*, UThinkGraphEdNode*> NodeMap;

	UPROPERTY(Transient)
	TMap<UThinkGraphEdge*, UThinkGraphEdNodeEdge*> EdgeMap;

	/** Shared ref to asset editor, namely to access debugger info and debugged node */
	TSharedPtr<FThinkGraphDebugger> Debugger;

	void RebuildGraphForEmbed(UThinkGraph* ThinkGraph, UThinkGraphEdNode_Embed* EmbedEdNode);
	
	virtual void RebuildGraph();

	UThinkGraph* GetThinkGraphModel() const;
	void            RebuildGraphForEdge(UThinkGraph* OwningGraph, UThinkGraphEdNodeEdge* EdGraphEdge);
	void            RebuildGraphForNode(UThinkGraph* OwningGraph, UThinkGraphEdNode* Node);
	void            RebuildGraphForLLM(UThinkGraph* OwningGraph, UThinkGraphEdNode_LLM* LLMEdNode);
	void            RebuildGraphForMemory(UThinkGraph* OwningGraph, UThinkGraphEdNode_Memory* MemEdNode);
	/** Goes through each nodes and run a validation pass */
	void ValidateNodes(FCompilerResultsLog* LogResults);

	//~ UObject interface
	virtual bool Modify(bool bAlwaysMarkDirty) override;
	virtual void PostEditUndo() override;
	//~ End UObject interface

	/** Returns all graph nodes that are of node type (not edges) */
	TArray<UThinkGraphEdNode*> GetAllNodes() const;

	/** Re-organize graph nodes automatically */
	void AutoArrange(bool bVertical);

protected:
	void Clear();

	// TODO: Is it needed anymore ?
	void SortNodes(UTGNode* RootNode);

	
	uint32 AnimIndex = 0;
};
