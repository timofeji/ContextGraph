// Created by Timofej Jermolaev, All rights reserved . 


#include "SThinkGraphNode_Embed.h"

#include "GraphEditorSettings.h"
#include "IDocumentation.h"
#include "SCommentBubble.h"
#include "SLevelOfDetailBranchNode.h"
#include "SThinkGraphPin.h"
#include "SThinkGraphPin_Value.h"
#include "ThinkGraphEditorStyle.h"
#include "ThinkGraphEditorTypes.h"
#include "TutorialMetaData.h"
#include "Graph/Nodes/ThinkGraphEdNode_Embed.h"

#define LOCTEXT_NAMESPACE "SThinkGraphNode_Embed"
/////////////////////////////////////////////////////
// SThinkGraphNode_Embed

void SThinkGraphNode_Embed::Construct(const FArguments& InArgs, UThinkGraphEdNode_Embed* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SThinkGraphNode_Embed::GetNodeInfoPopups(FNodeInfoContext* Context,
                                              TArray<FGraphInformationPopupInfo>& Popups) const
{
}

FSlateColor SThinkGraphNode_Embed::GetBorderBackgroundColor() const
{
	constexpr FLinearColor InactiveStateColor(1.f, 1.f, 1.f, 0.07f);
	return InactiveStateColor;
}

//
// void SThinkGraphNode_Embed::UpdateGraphNode()
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
//                     			.BorderBackgroundColor(this, &SThinkGraphNode_Embed::GetBorderBackgroundColor)
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

void SThinkGraphNode_Embed::AddValuePin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}
	
	ValuesBox->AddSlot()
	           .HAlign(HAlign_Fill)
	           .VAlign(VAlign_Top)
	           .FillHeight(1.0f)
	[
		PinToAdd
	];
	ValuePins.Add(PinToAdd);
}

void SThinkGraphNode_Embed::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	ValuePins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	ValuesBox.Reset();

	//
	//             ______________________
	//            |      TITLE AREA      |
	//            +-------+------+-------+
	//            | (>) L |      | R (>) |
	//            | (>) E |      | I (>) |
	//            | (>) F |      | G (>) |
	//            | (>) T |      | H (>) |
	//            |       |      | T (>) |
	//            |_______|______|_______|
	//			  |>>>>    VALUE  <<<<<<<|
	//            ________________________
	TSharedPtr<SVerticalBox> MainVerticalBox;
	SetupErrorReporting();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	// Get node icon
	IconColor = FLinearColor::White;
	const FSlateBrush* IconBrush = nullptr;
	if (GraphNode != NULL && GraphNode->ShowPaletteIconOnNode())
	{
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
	}

	TSharedRef<SOverlay> DefaultTitleAreaWidget =
		// SNew(SOverlay)
		// + SOverlay::Slot()
		// [
		// 	SNew(SImage)
		// 	.Image(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Node.Title"))
		// 	.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
		// ]

		SNew(SOverlay)
		+ SOverlay::Slot()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Node.Title.Overlay"))
				// .Padding(TitleBorderMargin)
						  .Padding(FMargin(1.f, 1.f, 1.f, 0.f))
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
				[
					SNew(SBorder)
				.BorderImage(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Node.Title.Overlay.Body"))
						  .Padding(FMargin(TitleBorderMargin))

		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Fill)
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
					[

						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						  .VAlign(VAlign_Top)
						  // .Padding(FMargin(0.f))
						  .Padding(FMargin(1.f, 1.f, 1.f, 0.f))
						  .AutoWidth()
						[
							SNew(SImage)
						.Image(IconBrush)
						.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							  .VAlign(VAlign_Center)
							  .HAlign(HAlign_Center)
							  .AutoHeight()
							[
								CreateTitleWidget(NodeTitle)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle.ToSharedRef()
							]
						]
					]
				]
			]
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Right)
			  .VAlign(VAlign_Center)
			  .Padding(0, 0, 0, 0)
			  .AutoWidth()
			[
				CreateTitleRightWidget()
			]
			// ]
			// + SOverlay::Slot()
			// .VAlign(VAlign_Top)
			// [
			// 	SNew(SBorder)
			// 	.Visibility(EVisibility::HitTestInvisible)
			// 	.BorderImage(FAppStyle::GetBrush("Graph.Node.TitleHighlight"))
			// 	.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleIconColor)
			// 	[
			// 		SNew(SSpacer)
			// 		.Size(FVector2D(20, 20))
			// 	]
		];

	SetDefaultTitleAreaWidget(DefaultTitleAreaWidget);

	const TSharedRef<SWidget> TitleAreaWidget =
		SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(this, &SThinkGraphNode_Embed::UseLowDetailNodeTitles)
			.LowDetail()
		[
			SNew(SBorder)
					.BorderImage(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Node.Title"))
					.Padding(FMargin(75.0f, 22.0f))
 // Saving enough space for a 'typical' title so the transition isn't quite so abrupt
					.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
		]
		.HighDetail()
		[
			DefaultTitleAreaWidget
		];


	if (!SWidget::GetToolTip().IsValid())
	{
		TSharedRef<SToolTip> DefaultToolTip = IDocumentation::Get()->CreateToolTip(
			TAttribute<FText>(this, &SGraphNode::GetNodeTooltip), NULL, GraphNode->GetDocumentationLink(),
			GraphNode->GetDocumentationExcerptName());
		SetToolTip(DefaultToolTip);
	}

	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("Graphnode"));
	PopulateMetaTag(&TagMeta);

	TSharedPtr<SVerticalBox> InnerVerticalBox;
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);


	InnerVerticalBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Top)
		  .Padding(Settings->GetNonPinNodeBodyPadding())
		[
			TitleAreaWidget
		]

		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Top)
		[
			CreateNodeContentArea()
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Bottom)
		[

			SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0, 3))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Left)
				  .FillWidth(1.0f)
				[
					// LEFT
					SAssignNew(ValuesBox, SVerticalBox)
				]
			]
		];

	TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
	if (EnabledStateWidget.IsValid())
	{
		InnerVerticalBox->AddSlot()
		                .AutoHeight()
		                .HAlign(HAlign_Fill)
		                .VAlign(VAlign_Top)
		                .Padding(FMargin(2, 0))
		[
			EnabledStateWidget.ToSharedRef()
		];
	}

	InnerVerticalBox->AddSlot()
	                .AutoHeight()
	                .Padding(Settings->GetNonPinNodeBodyPadding())
	[
		ErrorReporting->AsWidget()
	];

	InnerVerticalBox->AddSlot()
	                .AutoHeight()
	                .Padding(Settings->GetNonPinNodeBodyPadding())
	[
		VisualWarningReporting->AsWidget()
	];


	this->GetOrAddSlot(ENodeZone::Center)
	    .HAlign(HAlign_Center)
	    .VAlign(VAlign_Center)
	[
		SAssignNew(MainVerticalBox, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SOverlay)
			.AddMetaData<FGraphNodeMetaData>(TagMeta)
			+ SOverlay::Slot()
			.Padding(Settings->GetNonPinNodeBodyPadding())
			[
				SNew(SImage)
					.Image(GetNodeBodyBrush())
					.ColorAndOpacity(this, &SGraphNode::GetNodeTitleColor)
			]
			+ SOverlay::Slot()
			[
				InnerVerticalBox.ToSharedRef()
			]
		]
	];

	bool SupportsBubble = true;
	if (GraphNode != nullptr)
	{
		SupportsBubble = GraphNode->SupportsCommentBubble();
	}

	if (SupportsBubble)
	{
		// Create comment bubble
		TSharedPtr<SCommentBubble> CommentBubble;
		const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

		SAssignNew(CommentBubble, SCommentBubble)
			.GraphNode(GraphNode)
			.Text(this, &SGraphNode::GetNodeComment)
			.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
			.OnToggled(this, &SGraphNode::OnCommentBubbleToggled)
			.ColorAndOpacity(CommentColor)
			.AllowPinning(true)
			.EnableTitleBarBubble(true)
			.EnableBubbleCtrls(true)
			.GraphLOD(this, &SGraphNode::GetCurrentLOD)
			.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

		GetOrAddSlot(ENodeZone::TopCenter)
			.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
			.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
			.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
			.VAlign(VAlign_Top)
			[
				CommentBubble.ToSharedRef()
			];
	}

	CreateBelowWidgetControls(MainVerticalBox);
	CreatePinWidgets();
	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
	CreateBelowPinControls(InnerVerticalBox);
	CreateAdvancedViewArrow(InnerVerticalBox);
}

void SThinkGraphNode_Embed::CreatePinWidgets()
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		if (Pin->PinType.PinCategory == UThinkGraphPinNames::PinCategory_Value)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SThinkGraphPin, Pin);
			NewPin->SetShowLabel(true);
			AddValuePin(NewPin.ToSharedRef());
		}
		else
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SThinkGraphPin, Pin);
			NewPin->SetShowLabel(false);
			AddPin(NewPin.ToSharedRef());
		}
	}
}


FText SThinkGraphNode_Embed::GetPreviewCornerText() const
{
	return NSLOCTEXT("SThinkGraphNode_Embed", "CornerTextDescription", "Entry point for state machine");
}
#undef LOCTEXT_NAMESPACE
