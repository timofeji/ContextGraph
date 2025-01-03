#include "ThinkGraphEdNode_Embed.h"

#include "ThinkGraphEditorCommands.h"
#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Slate/SThinkGraphPin.h"

#define LOCTEXT_NAMESPACE "ThinkGraphEdNodeEntry"

UThinkGraphEdNode_Embed::UThinkGraphEdNode_Embed()
{
	bCanRenameNode = false;
}

void UThinkGraphEdNode_Embed::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UThinkGraphEdNode_Embed::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UThinkGraphPinNames::PinName_In, TEXT("Template"));
	CreatePin(EGPD_Output, UThinkGraphPinNames::PinName_Out, TEXT("Out"));

}


FSlateIcon UThinkGraphEdNode_Embed::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Embed");
}

bool UThinkGraphEdNode_Embed::ShowPaletteIconOnNode() const
{
	return true;
}


FLinearColor UThinkGraphEdNode_Embed::GetNodeTitleColor() const
{
	return FLinearColor(FColor::FromHex(TEXT("FF9AA2")));
}

FText UThinkGraphEdNode_Embed::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

void UThinkGraphEdNode_Embed::ExtractFromContextPrompt()
{
	// FString Result = Input;
	//
	// // Regular expression to match "${key}"
	// const FRegexPattern Pattern(TEXT(R"(\$\{([a-zA-Z0-9_]+)\})"));
	// FRegexMatcher Matcher(Pattern, Input);
	//
	// // Process all matches
	// while (Matcher.FindNext())
	// {
	// 	// Extract the key inside ${key}
	// 	FString Key = Matcher.GetCaptureGroup(1);
	//
	// 	// Check if the key exists in the map
	// 	if (const FString* Value = ValuesMap.Find(Key))
	// 	{
	// 		// Replace the match with the corresponding value
	// 		Result.ReplaceInline(*Matcher.GetCaptureGroup(0), **Value);
	// 	}
	// }
}


void UThinkGraphEdNode_Embed::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("AnimGraphNodeLayeredBoneblend", LOCTEXT("LayeredBoneBlend", "Layered Bone Blend"));
			if (Context->Pin != NULL)
			{
				// we only do this for normal BlendList/BlendList by enum, BlendList by Bool doesn't support add/remove pins
				if (Context->Pin->Direction == EGPD_Input)
				{
					//@TODO: Only offer this option on arrayed pins
					Section.AddMenuEntry(FThinkGraphEditorCommands::Get().RemoveValueBindPin);
				}
			}
			else
			{
				Section.AddMenuEntry(FThinkGraphEditorCommands::Get().AddValueBindPin);
			}
		}
	}
}
UEdGraphNode* UThinkGraphEdNode_Embed::GetOutputNode()
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

void UThinkGraphEdNode_Embed::CreateValueBind(const FString& Key)
{
	FScopedTransaction Transaction(FText::FromString(Key));
	Modify();

	ValueBindKeys.Add(Key);
	ReconstructNode();

	// FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UThinkGraphEdNode_Embed::ClearBinds()
{
	ValueBindKeys.Empty();
}
#undef LOCTEXT_NAMESPACE
