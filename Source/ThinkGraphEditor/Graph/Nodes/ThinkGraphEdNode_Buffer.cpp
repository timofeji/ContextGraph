#include "ThinkGraphEdNode_Buffer.h"

#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Buffer::UThinkGraphEdNode_Buffer()
{
	bCanRenameNode = false;
}

void UThinkGraphEdNode_Buffer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Buffer::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, TEXT("Memory"));
}

FText UThinkGraphEdNode_Buffer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Memory");
}

FLinearColor UThinkGraphEdNode_Buffer::GetNodeTitleColor() const
{
	return FLinearColor::Blue;
}

FSlateIcon UThinkGraphEdNode_Buffer::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Buffer");
}

bool UThinkGraphEdNode_Buffer::ShowPaletteIconOnNode() const
{
	return true;
}

FText UThinkGraphEdNode_Buffer::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UThinkGraphEdNode_Buffer::GetOutputNode()
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
