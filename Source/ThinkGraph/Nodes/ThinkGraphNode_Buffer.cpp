
#include "ThinkGraphNode_Buffer.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_Buffer::UThinkGraphNode_Buffer()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}



bool UThinkGraphNode_Buffer::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_Buffer::GetNodeTitle() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

#if WITH_EDITOR
#include "Preferences/PersonaOptions.h"
FText UThinkGraphNode_Buffer::GetAnimAssetLabel() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

FText UThinkGraphNode_Buffer::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


FLinearColor UThinkGraphNode_Buffer::GetBackgroundColor() const
{
	return  FLinearColor::Blue.Desaturate(0.24f);
}
#endif

#undef LOCTEXT_NAMESPACE
