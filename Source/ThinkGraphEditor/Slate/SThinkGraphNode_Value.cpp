// Created by Timofej Jermolaev, All rights reserved . 


#include "SThinkGraphNode_Value.h"

#include "GraphEditorSettings.h"
#include "IDocumentation.h"
#include "SCommentBubble.h"
#include "SLevelOfDetailBranchNode.h"
#include "ThinkGraphEditorStyle.h"
#include "Widgets/SBoxPanel.h"
#include "SThinkGraphPin.h"
#include "TutorialMetaData.h"
#include "Graph/Nodes/ThinkGraphEdNode_Const.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SThinkGraphNode_Prompt"
/////////////////////////////////////////////////////
// SThinkGraphNode_Prompt

void SThinkGraphNode_Value::Construct(const FArguments& InArgs, UThinkGraphEdNode* InNode)
{
	SThinkGraphNode::Construct(SThinkGraphNode::FArguments(), InNode);

	GraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	//
	UpdateGraphNode();
}

void SThinkGraphNode_Value::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

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

	TSharedPtr<SHorizontalBox> InnerBox;
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);


	TSharedRef<SOverlay> DefaultTitleAreaWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Center)
		  .Padding(FMargin(3.f, 0.f))
		[

			SNew(SBorder)
				.Padding(FMargin(0.f))
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .VAlign(VAlign_Center)
				  // .Padding(FMargin(1.f, 1.f, 1.f, 0.f))
				  .AutoWidth()
				[
					SNew(SImage)
						.Image(IconBrush)
						.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
				]
				+ SHorizontalBox::Slot()
				  .Padding(FMargin(3.f, 0.f))
				  .VAlign(VAlign_Center)
				  .HAlign(HAlign_Center)
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
					  .VAlign(VAlign_Center)
					  .HAlign(HAlign_Center)
					[
						NodeTitle.ToSharedRef()
					]
				]
			]

		];

	InnerBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .Padding(FMargin(1.f, 0.0f))
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Fill)
		[

			SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0, 0))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Left)
				  .FillWidth(1.0f)
				[
					// LEFT
					SAssignNew(LeftNodeBox, SVerticalBox)
				]


				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Fill)
				  .Padding(FMargin(.0f, 0.0f))
				[
					DefaultTitleAreaWidget
				]
				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  .HAlign(HAlign_Right)
				[
					// RIGHT
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			]
		];


	TSharedPtr<SWidget> EnabledStateWidget = GetEnabledStateWidget();
	if (EnabledStateWidget.IsValid())
	{
		InnerBox->AddSlot()
		        .AutoWidth()
		        .HAlign(HAlign_Fill)
		        .VAlign(VAlign_Top)
			// .Padding(FMargin(2, 2))
			[
				EnabledStateWidget.ToSharedRef()
			];
	}

	InnerBox->AddSlot()
	        .AutoWidth()
	        .Padding(Settings->GetNonPinNodeBodyPadding())
	[
		ErrorReporting->AsWidget()
	];

	InnerBox->AddSlot()
	        .AutoWidth()
	        .Padding(Settings->GetNonPinNodeBodyPadding())
	[
		VisualWarningReporting->AsWidget()
	];


	this->GetOrAddSlot(ENodeZone::Center)
	    .Padding(FMargin(0.f, 0.0f))
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
			// .Padding(Settings->GetNonPinNodeBodyPadding())
			[
				SNew(SImage)
					.Image(GetNodeBodyBrush())
					.ColorAndOpacity(this, &SGraphNode::GetNodeTitleColor)
			]
			+ SOverlay::Slot()
			[
				InnerBox.ToSharedRef()
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
	// CreateBelowPinControls(InnerVerticalBox);
	// CreateAdvancedViewArrow(InnerVerticalBox);
}

const FSlateBrush* SThinkGraphNode_Value::GetNodeBodyBrush() const
{
	return FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Node.Value.Body"));
}


void SThinkGraphNode_Value::GetNodeInfoPopups(FNodeInfoContext* Context,
                                              TArray<FGraphInformationPopupInfo>& Popups) const
{
}

FText SThinkGraphNode_Value::GetPreviewCornerText() const
{
	return NSLOCTEXT("SThinkGraphNode_Prompt", "CornerTextDescription", "Entry point for state machine");
}
#undef LOCTEXT_NAMESPACE
