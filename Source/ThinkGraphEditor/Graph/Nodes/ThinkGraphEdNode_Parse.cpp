#include "ThinkGraphEdNode_Parse.h"

#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Parse::UThinkGraphEdNode_Parse()
{
	bCanRenameNode = false;
}

void UThinkGraphEdNode_Parse::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Parse::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, TEXT("BasePrompt"));
	CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, TEXT("ContextPrompt"));
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("ParsedPrompt"));
}

FText UThinkGraphEdNode_Parse::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Parse");
}

FLinearColor UThinkGraphEdNode_Parse::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("ffca3a")));
}
FText UThinkGraphEdNode_Parse::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Parses an injected prompt into the base prompt");
}

FSlateIcon UThinkGraphEdNode_Parse::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Parse");
}

bool UThinkGraphEdNode_Parse::ShowPaletteIconOnNode() const
{
	return true;
}


UEdGraphNode* UThinkGraphEdNode_Parse::GetOutputNode()
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
