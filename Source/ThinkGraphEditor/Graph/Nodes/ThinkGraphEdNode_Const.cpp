#include "ThinkGraphEdNode_Const.h"

#include "ThinkGraphEditorTypes.h"
#include "ThinkGraphEdNode_Embed.h"
#include "Graph/ThinkGraphEdGraph.h"

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
	return LOCTEXT("ConstNodeTooltip", "Add a constant buffer");
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

void UThinkGraphEdNode_Const::OnDataUpdated()
{
	const auto Pin = GetOutputPin();
	Pin->PinType.bIsConst = true;

	if(auto TGEdGraph = CastChecked<UThinkGraphEdGraph>(GetGraph()))
	{
		TGEdGraph->PopulateBuffersForConst(TGEdGraph->GetThinkGraphModel(), this);
	}

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
