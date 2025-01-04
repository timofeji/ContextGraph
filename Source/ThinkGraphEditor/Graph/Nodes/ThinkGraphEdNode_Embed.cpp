#include "ThinkGraphEdNode_Embed.h"

#include "ThinkGraphEditorCommands.h"
#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Slate/SThinkGraphPin.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Embed.h"

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


void UThinkGraphEdNode_Embed::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("AnimGraphNodeLayeredBoneblend",
			                                             LOCTEXT("LayeredBoneBlend", "Layered Bone Blend"));
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

void UThinkGraphEdNode_Embed::ReallocateBindPins(TArray<FString>& NewKeys)
{
	if (NewKeys.IsEmpty())
	{
		return;
	}


	FScopedTransaction Transaction(FText::FromString(NewKeys[0]));
	Modify();

	TArray<FString> OldKeys = ValueBindKeys.Array();
	for (int i = 0; i < OldKeys.Num(); i++)
	{
		//Key Removed
		if (!NewKeys.Contains(OldKeys[i]))
		{
			RemovePinAt(i, EGPD_Input);
		}
	}

	ValueBindKeys.Reset();
	for (auto Key : NewKeys)
	{
		ValueBindKeys.Add(Key);
		if (!OldKeys.Contains(Key))
		{
			CreatePin(EGPD_Input, UThinkGraphPinNames::PinCategory_Value, FName(Key));
		}
	}

	// FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UThinkGraphEdNode_Embed::ClearBinds()
{
}
#undef LOCTEXT_NAMESPACE
