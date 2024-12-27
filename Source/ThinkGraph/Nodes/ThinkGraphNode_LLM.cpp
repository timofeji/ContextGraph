
#include "ThinkGraphNode_LLM.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_LLM::UThinkGraphNode_LLM()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}



bool UThinkGraphNode_LLM::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_LLM::GetNodeTitle() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

#if WITH_EDITOR
#include "Preferences/PersonaOptions.h"
FText UThinkGraphNode_LLM::GetAnimAssetLabel() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

FText UThinkGraphNode_LLM::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


FLinearColor UThinkGraphNode_LLM::GetBackgroundColor() const
{
	return  FLinearColor::Blue.Desaturate(0.24f);
}
#endif

#undef LOCTEXT_NAMESPACE
