#include "ThinkGraphEdNode_Call.h"

#include "ThinkGraphEditorTypes.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeCall"

UThinkGraphEdNode_Call::UThinkGraphEdNode_Call()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_Call::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Call::AllocateDefaultPins()
{
}


FLinearColor UThinkGraphEdNode_Call::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("27332b")));
}

FText UThinkGraphEdNode_Call::GetTooltipText() const
{
	return LOCTEXT("StateCallNodeTooltip", "Request a Call execution");
}

UEdGraphNode* UThinkGraphEdNode_Call::GetOutputNode()
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
