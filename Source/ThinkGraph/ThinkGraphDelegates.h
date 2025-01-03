#pragma once

#include "CoreMinimal.h"

class APawn;
class UThinkGraph;
class UTGNode;
class UThinkGraphComponent;

struct THINKGRAPH_API FThinkGraphDelegates
{
	/** delegate type for combo graph execution events (Params: const UComboGraphAbilityTask_StartGraph*, const UComboGraph* ComboGraphAsset) */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphEvaluated,
	                                     const UTGNode& Node);
	
	DECLARE_MULTICAST_DELEGATE_OneParam(ConstNodeUpdated,
	                                     const UTGNode& Node);


	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphReset,
	                                     const UThinkGraphComponent& Component);


	/** Called when the Graph activates a node*/
	static FOnThinkGraphEvaluated OnNodeStartedGenerating;

	
	/** Called when the Graph tries to find a node. */
	static ConstNodeUpdated OnConstGraphUpdated;

	/** Called when the Combo Graph ends execution. */
	static FOnThinkGraphReset OnGraphReset;


	/** delegate type for combo graph execution events (Params: const UComboGraphAbilityTask_StartGraph*, const UComboGraph* ComboGraphAsset) */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphNodeEvaluated,
	                                     const UTGNode *Node);

	static FOnThinkGraphNodeEvaluated OnNodeEvaluated;

#if WITH_EDITORONLY_DATA

#endif
};
