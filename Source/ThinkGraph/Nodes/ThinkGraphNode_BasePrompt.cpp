
#include "ThinkGraphNode_BasePrompt.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_BasePrompt::UThinkGraphNode_BasePrompt()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Montage Action Node");
#endif
}



bool UThinkGraphNode_BasePrompt::SupportsAssetClass(UClass* AssetClass)
{
	return AssetClass->IsChildOf(UAnimSequence::StaticClass());
}

FText UThinkGraphNode_BasePrompt::GetNodeTitle() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

#if WITH_EDITOR
#include "Preferences/PersonaOptions.h"
FText UThinkGraphNode_BasePrompt::GetAnimAssetLabel() const
{
	return LOCTEXT("BasePromptLabel", "BasePrompt");
}

FText UThinkGraphNode_BasePrompt::GetAnimAssetLabelTooltip() const
{
	return LOCTEXT("AnimAssetLabelTooltip", "Sequence");
}


FLinearColor UThinkGraphNode_BasePrompt::GetBackgroundColor() const
{
	return  FLinearColor::Blue.Desaturate(0.24f);
}
#endif

#undef LOCTEXT_NAMESPACE
