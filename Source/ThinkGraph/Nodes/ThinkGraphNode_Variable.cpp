#include "ThinkGraphNode_Variable.h"

#include "TGTypes.h"
#include "ThinkGraph.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeVariable"

UThinkGraphNode_Variable::UThinkGraphNode_Variable()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Variable Context Node");
#endif
}

#undef LOCTEXT_NAMESPACE
