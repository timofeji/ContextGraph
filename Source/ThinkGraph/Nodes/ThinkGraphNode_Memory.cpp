#include "ThinkGraphNode_Memory.h"

#include "ThinkGraphNode_LLM.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Memory::UThinkGraphNode_Memory()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeMemory_ContextMenuName", "ThinkGraph Memory Node");
#endif
}

#if WITH_EDITOR
void UThinkGraphNode_Memory::RecallMemory()
{
	//we just wanna invalidate the data buffer and have the system automatically populate it
	if(InBufferIDS.Num() > 0)
	{
		
	}
	
	bIsGenerating = true;
}

FLinearColor UThinkGraphNode_Memory::GetBackgroundColor() const
{
	return FLinearColor::Blue.Desaturate(0.24f);
}
#endif

#undef LOCTEXT_NAMESPACE
