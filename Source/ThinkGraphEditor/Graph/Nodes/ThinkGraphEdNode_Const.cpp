#include "ThinkGraphEdNode_Const.h"

#include "ThinkGraphEditorTypes.h"
#include "ThinkGraphEdNode_Embed.h"

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

void UThinkGraphEdNode_Const::UpdateEmbeddedKeys()
{
	FString PromptStr = Text;
	// Regular expression to match "${key}"
	const FRegexPattern Pattern(TEXT(R"(\$\{([a-zA-Z0-9_]+)\})"));
	FRegexMatcher Matcher(Pattern, PromptStr);

	// Process all matches
	while (Matcher.FindNext())
	{
		// Extract the key inside ${key}
		FString Key = Matcher.GetCaptureGroup(1);
		EmbeddedKeys.AddUnique(Key);
	}

	const auto Pin = GetOutputPin();
	Pin->PinType.bIsConst = true;

	for (const auto LinkedToPin : Pin->LinkedTo)
	{
		LinkedToPin->PinType.bIsConst = true;
	}

	
}

#undef LOCTEXT_NAMESPACE
