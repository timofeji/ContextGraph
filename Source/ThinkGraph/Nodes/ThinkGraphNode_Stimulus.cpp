
#include "ThinkGraphNode_Stimulus.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeStimulus"

UThinkGraphNode_Stimulus::UThinkGraphNode_Stimulus()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNode_ContextMenuName", "Stimulus Node");
#endif
}

void UThinkGraphNode_Stimulus::Activate(UThinkGraph* ThinkGraph)
{
	Super::Activate(ThinkGraph);

	
}


#undef LOCTEXT_NAMESPACE
