#include "ThinkGraphEdNode_BasePrompt.h"

#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_BasePrompt::UThinkGraphEdNode_BasePrompt()
{
	bCanRenameNode = false;
}

void UThinkGraphEdNode_BasePrompt::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_BasePrompt::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("Prompt"));
}

FText UThinkGraphEdNode_BasePrompt::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Prompt");
}

FLinearColor UThinkGraphEdNode_BasePrompt::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("27332b")));
}

FText UThinkGraphEdNode_BasePrompt::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UThinkGraphEdNode_BasePrompt::GetOutputNode()
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
