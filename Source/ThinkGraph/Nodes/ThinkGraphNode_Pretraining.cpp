
#include "ThinkGraphNode_Pretraining.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Pretraining::UThinkGraphNode_Pretraining()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}

#if WITH_EDITOR


FLinearColor UThinkGraphNode_Pretraining::GetBackgroundColor() const
{
	return  FLinearColor::Blue.Desaturate(0.24f);
}
#endif

#undef LOCTEXT_NAMESPACE
