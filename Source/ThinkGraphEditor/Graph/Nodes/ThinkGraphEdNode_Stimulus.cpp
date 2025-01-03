#include "ThinkGraphEdNode_Stimulus.h"

#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Stimulus::UThinkGraphEdNode_Stimulus()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_Stimulus::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Stimulus::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_In, TEXT("Stimulus"));
}

FLinearColor UThinkGraphEdNode_Stimulus::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("FF6961")));
}

FSlateIcon UThinkGraphEdNode_Stimulus::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Stimulus");
}

bool UThinkGraphEdNode_Stimulus::ShowPaletteIconOnNode() const
{
	return true;
}

FText UThinkGraphEdNode_Stimulus::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UThinkGraphEdNode_Stimulus::GetOutputNode()
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
