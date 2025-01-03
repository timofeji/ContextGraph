
#include "ThinkGraphNode_Embed.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNode_Embed"

UThinkGraphNode_Embed::UThinkGraphNode_Embed()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Embed Buffer");
#endif
}

void UThinkGraphNode_Embed::AddPinValuePin()
{
}

#undef LOCTEXT_NAMESPACE
