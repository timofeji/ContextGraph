#include "ThinkGraphEdNode_LLM.h"

#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"
#include "..\..\..\ThinkGraph\Nodes\TGNode.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_LLM.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_LLM::UThinkGraphEdNode_LLM()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_LLM::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_LLM::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("Output"));

	if (const auto LLMNode = Cast<UThinkGraphNode_LLM>(RuntimeNode))
	{
		for (int i = 0; i < LLMNode->InputRoles.Num(); i++)
		{
			CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, FName(LLMNode->InputRoles[i]));
		}
	}
}


FLinearColor UThinkGraphEdNode_LLM::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("8ac926")));
}

FSlateIcon UThinkGraphEdNode_LLM::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.LLM");
}

bool UThinkGraphEdNode_LLM::ShowPaletteIconOnNode() const
{
	return true;
}

FText UThinkGraphEdNode_LLM::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip",
	               "Generative Language Model -- Takes in message prompts and outputs a response");
}

UEdGraphNode* UThinkGraphEdNode_LLM::GetOutputNode()
{
	if (Pins.Num() > 0 && Pins[0] != nullptr)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if (Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != nullptr)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
