#pragma once

#include "CoreMinimal.h"

class APawn;
class UThinkGraph;
class UThinkGraphNode;
class UThinkGraphComponent;

struct THINKGRAPH_API FThinkGraphDelegates
{
	/** delegate type for combo graph execution events (Params: const UComboGraphAbilityTask_StartGraph*, const UComboGraph* ComboGraphAsset) */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnThinkGraphEvaluated,
	                                     const UThinkGraphComponent& Component,
	                                     const UThinkGraphNode& Node);
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphSelected,
	                                     const UThinkGraphNode& ActionComponent);


	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphReset,
	                                     const UThinkGraphComponent& Component);


	/** Called when the Graph activates a node*/
	static FOnThinkGraphEvaluated OnGraphNodeEvaluated;

	
	/** Called when the Graph tries to find a node. */
	static FOnThinkGraphSelected OnGraphNodeSelected;

	/** Called when the Combo Graph ends execution. */
	static FOnThinkGraphReset OnGraphReset;


	/** delegate type for combo graph execution events (Params: const UComboGraphAbilityTask_StartGraph*, const UComboGraph* ComboGraphAsset) */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnThinkGraphNodeEvaluated,
	                                     const UThinkGraphNode *Node);

	static FOnThinkGraphNodeEvaluated OnNodeEvaluated;

#if WITH_EDITORONLY_DATA

#endif
};
