#include "ThinkGraphNode_Const.h"

#include "TGTypes.h"
#include "ThinkGraph.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Const::UThinkGraphNode_Const()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}

#undef LOCTEXT_NAMESPACE
