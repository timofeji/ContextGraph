#include "ThinkGraphEdNode_Variable.h"

#include "ThinkGraphEditorTypes.h"
#include "ThinkGraphEdNode_Embed.h"
#include "Graph/ThinkGraphEdGraph.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeVariable"

UThinkGraphEdNode_Variable::UThinkGraphEdNode_Variable()
{
	bCanRenameNode = true;
}

void UThinkGraphEdNode_Variable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Variable::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("Prompt"));
}


FLinearColor UThinkGraphEdNode_Variable::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("27332b")));
}

FText UThinkGraphEdNode_Variable::GetTooltipText() const
{
	return LOCTEXT("VariableNodeTooltip", "Add a constant buffer");
}

UEdGraphNode* UThinkGraphEdNode_Variable::GetOutputNode()
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

void UThinkGraphEdNode_Variable::OnDataUpdated()
{
	const auto Pin = GetOutputPin();
	Pin->PinType.bIsConst = true;

	for (const auto LinkedToPin : Pin->LinkedTo)
	{
		LinkedToPin->PinType.bIsConst = true;
		
		if (auto EmbedNode = Cast<UThinkGraphEdNode_Embed>(LinkedToPin->GetOwningNode()))
		{
			EmbedNode->OnTemplateUpdated();
		}
	}
}

#undef LOCTEXT_NAMESPACE
