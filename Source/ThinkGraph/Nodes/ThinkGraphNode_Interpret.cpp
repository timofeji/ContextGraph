
#include "ThinkGraphNode_Interpret.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Interpret::UThinkGraphNode_Interpret()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}



bool UThinkGraphNode_Interpret::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_Interpret::GetNodeTitle() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

#if WITH_EDITOR
FText UThinkGraphNode_Interpret::GetAnimAssetLabel() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

FText UThinkGraphNode_Interpret::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


#endif

#undef LOCTEXT_NAMESPACE
