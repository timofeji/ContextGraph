
#include "ThinkGraphNode_Parse.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Parse::UThinkGraphNode_Parse()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}



bool UThinkGraphNode_Parse::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_Parse::GetNodeTitle() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

#if WITH_EDITOR
FText UThinkGraphNode_Parse::GetAnimAssetLabel() const
{
	return LOCTEXT("ParseNodeLabel", "ParseNode");
}

FText UThinkGraphNode_Parse::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


#endif

#undef LOCTEXT_NAMESPACE
