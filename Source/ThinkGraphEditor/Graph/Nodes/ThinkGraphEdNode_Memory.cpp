#include "ThinkGraphEdNode_Memory.h"

#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Memory::UThinkGraphEdNode_Memory()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_Memory::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Memory::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, TEXT("Memory"));
}

FLinearColor UThinkGraphEdNode_Memory::GetNodeTitleColor() const
{
	return FLinearColor::Blue;
}

FSlateIcon UThinkGraphEdNode_Memory::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Memory");
}

bool UThinkGraphEdNode_Memory::ShowPaletteIconOnNode() const
{
	return true;
}

FText UThinkGraphEdNode_Memory::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UThinkGraphEdNode_Memory::GetInputNode()
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
