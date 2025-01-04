// Created by Timofej Jermolaev, All rights reserved . 


#include "SThinkGraphNode_Const.h"

#include "ThinkGraphEditorStyle.h"
#include "Widgets/SBoxPanel.h"
#include "SThinkGraphPin.h"
#include "Graph/Nodes/ThinkGraphEdNode_Const.h"

#define LOCTEXT_NAMESPACE "SThinkGraphNode_Prompt"
/////////////////////////////////////////////////////
// SThinkGraphNode_Prompt

void SThinkGraphNode_Const::Construct(const FArguments& InArgs, UThinkGraphEdNode_Const* InNode)
{

	SThinkGraphNode::Construct(SThinkGraphNode::FArguments(), InNode);
	
	GraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	//
	// UpdateGraphNode();
}

void SThinkGraphNode_Const::GetNodeInfoPopups(FNodeInfoContext* Context,
                                                TArray<FGraphInformationPopupInfo>& Popups) const
{
}

FSlateColor SThinkGraphNode_Const::GetBorderBackgroundColor() const
{
	constexpr FLinearColor InactiveStateColor(1.f, 1.f, 1.f, 0.07f);
	return InactiveStateColor;
}
//
// void SThinkGraphNode_Prompt::UpdateGraphNode()
// {
// 	InputPins.Empty();
// 	OutputPins.Empty();
//
// 	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
// 	RightNodeBox.Reset();
//
// 	//
// 	//             ______________________
// 	//            |      TITLE AREA      |
// 	//            +-------+------+-------+
// 	//            | (>) L |      | R (>) |
// 	//            | (>) E |      | I (>) |
// 	//            | (>) F |      | G (>) |
// 	//            | (>) T |      | H (>) |
// 	//            |       |      | T (>) |
// 	//            |_______|______|_______|
// 	//
//
// 	TSharedPtr<SErrorText> ErrorText;
// 	constexpr FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f, 0.3f);
//
// 	// const TSharedPtr<SVerticalBox> NodeContent = CreateNodeContent();
//
// 	ContentScale.Bind(this, &SGraphNode::GetContentScale);
// 	GetOrAddSlot(ENodeZone::Center)
// 		.HAlign(HAlign_Fill)
// 		.VAlign(VAlign_Fill)
// 		[
// 			SNew(SOverlay)
// 			+ SOverlay::Slot()
// 			  .HAlign(HAlign_Fill)
// 			  .VAlign(VAlign_Fill)
// 			[
// 				SNew(SBorder)
//                     			// .BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
//                     			.BorderImage(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Icon.Prompt"))
//                     			.Padding(10.f)
//                     			.DesiredSizeScale(FVector2d(1.f, 1.f))
//                     			.BorderBackgroundColor(this, &SThinkGraphNode_Prompt::GetBorderBackgroundColor)
// 				[
// 					SAssignNew(RightNodeBox, SVerticalBox)
// 				]
// 			]
//
// 		];
//
// 	// // Create comment bubble
// 	// TSharedPtr<SCommentBubble> CommentBubble;
// 	// const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
// 	//
// 	// SAssignNew(CommentBubble, SCommentBubble)
// 	//    		.GraphNode(GraphNode)
// 	//    		.Text(this, &SGraphNode::GetNodeComment)
// 	//    		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
// 	//    		.ColorAndOpacity(CommentColor)
// 	//    		.AllowPinning(true)
// 	//    		.EnableTitleBarBubble(true)
// 	//    		.EnableBubbleCtrls(true)
// 	//    		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
// 	//    		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);
// 	//
// 	// GetOrAddSlot(ENodeZone::TopCenter)
// 	// 	.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
// 	// 	.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
// 	// 	.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
// 	// 	.VAlign(VAlign_Top)
// 	// 	[
// 	// 		CommentBubble.ToSharedRef()
// 	// 	];
//
// 	// ErrorReporting = ErrorText;
// 	// ErrorColor = FAppStyle::GetColor("ErrorReporting.BackgroundColor");
// 	// ErrorReporting->SetError(ErrorMsg);
//
// 	CreatePinWidgets();
// }
// //

// void SThinkGraphNode_Prompt::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
// {
// 	PinToAdd->SetOwner(SharedThis(this));
// 	PinToAdd->SetShowLabel(false);
// 	RightNodeBox->AddSlot()
// 	            .HAlign(HAlign_Fill)
// 	            .VAlign(VAlign_Fill)
// 	            .FillHeight(1.0f)
// 	[
// 		PinToAdd
// 	];
// 	
// 	OutputPins.Add(PinToAdd);
// }
//
// void SThinkGraphNode_Prompt::CreatePinWidgets()
// {
// 	for (UEdGraphPin* Pin : GraphNode->Pins)
// 	{
// 		if (Pin->Direction == EGPD_Output)
// 		{
// 			TSharedPtr<SGraphPin> NewPin = SNew(SThinkGraphPin, Pin);
// 			NewPin->SetShowLabel(false);
// 			AddPin(NewPin.ToSharedRef());
// 		}
// 	}
// }
//
FText SThinkGraphNode_Const::GetPreviewCornerText() const
{
	return NSLOCTEXT("SThinkGraphNode_Prompt", "CornerTextDescription", "Entry point for state machine");
}
#undef LOCTEXT_NAMESPACE
