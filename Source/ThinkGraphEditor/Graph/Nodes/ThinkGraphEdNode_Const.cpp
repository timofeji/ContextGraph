#include "ThinkGraphEdNode_Const.h"

#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Const::UThinkGraphEdNode_Const()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_Const::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Const::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("Prompt"));
}


FLinearColor UThinkGraphEdNode_Const::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("27332b")));
}

FText UThinkGraphEdNode_Const::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UThinkGraphEdNode_Const::GetOutputNode()
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
