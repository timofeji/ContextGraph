
#include "ThinkGraphNode_Distribution.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Distribution::UThinkGraphNode_Distribution()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Distribution Node");
#endif
}



bool UThinkGraphNode_Distribution::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_Distribution::GetNodeTitle() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

#if WITH_EDITOR
FText UThinkGraphNode_Distribution::GetAnimAssetLabel() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

FText UThinkGraphNode_Distribution::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


#endif

#undef LOCTEXT_NAMESPACE
