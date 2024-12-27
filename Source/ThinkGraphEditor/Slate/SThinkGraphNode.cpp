#include "SThinkGraphNode.h"

#include "SCommentBubble.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"
#include "IDocumentation.h"
#include "SThinkGraphPin.h"
#include "ThinkGraphDebugger.h"
#include "ThinkGraphEditorStyle.h"
#include "TutorialMetaData.h"
#include "Editor/KismetWidgets/Public/SLevelOfDetailBranchNode.h"
#include "Graph/ThinkGraphDragDropAction.h"
#include "Graph/ThinkGraphEdGraph.h"
#include "Graph/Nodes/ThinkGraphEdNode.h"


#include "ThinkGraph/Nodes/ThinkGraphNode.h"

#define LOCTEXT_NAMESPACE "SThinkGraphNode"

//////////////////////////////////////////////////////////////////////////
void SThinkGraphNode::Construct(const FArguments& InArgs, UThinkGraphEdNode* InNode)
{
	check(InNode)
	GraphNode = InNode;
	CachedGraphNode = InNode;
	ContentPadding = InArgs._ContentPadding;
	ContentInternalPadding = InArgs._ContentInternalPadding;
	PinSize = InArgs._PinSize;
	PinPadding = InArgs._PinPadding;
	bDrawVerticalPins = InArgs._DrawVerticalPins;
	bDrawHorizontalPins = InArgs._DrawHorizontalPins;
	UpdateGraphNode();
	SetCursor(EMouseCursor::CardinalCross);
	InNode->SlateNode = this;
}

void SThinkGraphNode::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNode::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (CachedGraphNode)
	{
		CachedGraphNode->UpdateTime(InDeltaTime);
	}
}

//
void SThinkGraphNode::UpdateGraphNode()
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
	//
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
			.UseLowDetailSlot(this, &SThinkGraphNode::UseLowDetailNodeTitles)
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

void SThinkGraphNode::CreatePinWidgets()
{
	if (GraphNode->Pins.Num() < 2)
	{
		bIsDataPointNode = true;
	}

	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SThinkGraphPin, Pin);
		if (bIsDataPointNode)
		{
			NewPin->SetShowLabel(false);
		}

		AddPin(NewPin.ToSharedRef());
	}
}


void SThinkGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
		           .HAlign(HAlign_Fill)
		           .VAlign(VAlign_Top)
		           .FillHeight(1.0f)
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		RightNodeBox->AddSlot()
		            .HAlign(HAlign_Fill)
		            .VAlign(VAlign_Top)
		            .FillHeight(1.0f)
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
	}
}

bool SThinkGraphNode::IsNameReadOnly() const
{
	UThinkGraphEdNode* EdNode_Node = Cast<UThinkGraphEdNode>(GraphNode);
	check(EdNode_Node != nullptr);

	return (!EdNode_Node->RuntimeNode->IsNameEditable()) || SGraphNode::IsNameReadOnly();
}

const FSlateBrush* SThinkGraphNode::GetShadowBrush(bool bSelected) const
{
	return bSelected
		       ? FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Node.ActiveShadow"))
		       : FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Node.Shadow"));
}

const FSlateBrush* SThinkGraphNode::GetNodeBodyBrush() const
{
	return FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Node.Body"));
}

// const FSlateBrush* SThinkGraphNode::GetNodeTitleBrush() const
// {
// 	return FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Node.Title"));
// }

FSlateColor SThinkGraphNode::GetNodeTitleColor() const
{
	FLinearColor ReturnTitleColor = GraphNode->IsDeprecated() ? FLinearColor::Red : GetNodeObj()->GetNodeTitleColor();

	// if (FlowGraphNode->GetSignalMode() == EFlowSignalMode::Enabled)
	// {
	// 	ReturnTitleColor.A = FadeCurve.GetLerp();
	// }
	// else
	// {
	// 	ReturnTitleColor *= FLinearColor(0.5f, 0.5f, 0.5f, 0.4f);
	// }
	//
	// if (!IsFlowGraphNodeSelected(FlowGraphNode) && FlowGraphNode->IsSubNode())
	// {
	// 	ReturnTitleColor *= UnselectedNodeTint;
	// }

	return ReturnTitleColor;
}

int32 SThinkGraphNode::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                               const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                               int32 LayerId,
                               const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	uint8 DebugMode = MG_DEBUG_NONE;
	auto ThinkEdGraph = Cast<UThinkGraphEdGraph>(GraphNode->GetGraph());
	if (ThinkEdGraph && ThinkEdGraph->Debugger.IsValid())
	{
		FThinkGraphDebugger* Debugger = ThinkEdGraph->Debugger.Get();
		if (Debugger && Debugger->IsDebuggerReady())
		{
			DebugMode = Debugger->GetDebugMode();
		}
	}


	return SGraphNode::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                           bParentEnabled);
}


//
// FSlateColor SThinkGraphNode::GetBorderBackgroundColor() const
// {
// 	const UThinkGraphEdNode* MyNode = CastChecked<UThinkGraphEdNode>(GraphNode);
// 	return MyNode->GetBackgroundColor();
// }

EVisibility SThinkGraphNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SThinkGraphNode::GetNameIcon() const
{
	return FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Icon"));
}

void SThinkGraphNode::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	if (!CachedGraphNode)
	{
		return;
	}

	const UThinkGraphNode* DebuggedNode = CachedGraphNode->GetDebuggedNode();
	if (!DebuggedNode)
	{
		return;
	}

	if (CachedGraphNode->IsDebugActive())
	{
		const FString Message = FString::Printf(TEXT("Active for %.2f secs"), CachedGraphNode->GetDebugTotalTime());
		new(Popups) FGraphInformationPopupInfo(nullptr, CachedGraphNode->GetBackgroundColor(), Message);

		const FString Message2 = FString::Printf(TEXT("Evaluated in %.5f secs"), CachedGraphNode->GetEvaluationTime());
		new(Popups) FGraphInformationPopupInfo(nullptr, CachedGraphNode->GetBackgroundColor().Desaturate(0.2f),
		                                       Message2);
	}
	else if (CachedGraphNode->WasDebugActive())
	{
		const FString Message = FString::Printf(
			TEXT("Was Active for %.2f secs"), CachedGraphNode->GetDebugElapsedTime());
		new(Popups) FGraphInformationPopupInfo(nullptr, CachedGraphNode->GetBackgroundColor(), Message);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void SThinkGraphNode::SetErrorText(const FText InErrorText)
{
	if (ErrorReporting)
	{
		ErrorReporting->SetError(InErrorText);
	}
}

TSharedPtr<SVerticalBox> SThinkGraphNode::CreateNodeContent()
{
	return nullptr;
	// 	TSharedPtr<SVerticalBox> Content = SNew(SVerticalBox);
	// 	const TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	//
	// 	Content->AddSlot()
	// 	       .AutoHeight()
	// 	       .HAlign(HAlign_Fill)
	// 	[
	// 		SAssignNew(InlineEditableText, SInlineEditableTextBlock)
	// 		.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
	// 		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
	// 		.OnVerifyTextChanged(this, &SThinkGraphNode::OnVerifyNameTextChanged)
	// 		.OnTextCommitted(this, &SThinkGraphNode::OnNameTextCommited)
	// 		.IsReadOnly(this, &SThinkGraphNode::IsNameReadOnly)
	// 		.IsSelected(this, &SThinkGraphNode::IsSelectedExclusively)
	// 		.Justification(ETextJustify::Center)
	// 	];
	//
	// 	Content->AddSlot()
	// 	       .AutoHeight()
	// 	[
	// 		NodeTitle.ToSharedRef()
	// 	];
	//
	// 	if (!CachedGraphNode || !CachedGraphNode->RuntimeNode)
	// 	{
	// 		return Content;
	// 	}
	//
	// 	const FMargin RowPadding = FMargin(0.f, 0.f, 0.f, 0.f);
	// 	const FMargin TopRowPadding = FMargin(0.f, 0.f, 0.f, 0.f);
	// 	const FMargin BottomRowPadding = FMargin(0.f, 0.f, 0.f, 0.f);
	//
	// 	Content->AddSlot()
	// 	       .Padding(TopRowPadding)
	// 	       .AutoHeight()
	// 	[
	// 		SNew(SThinkGraphNodeRow, CachedGraphNode->RuntimeNode)
	// 		.IsTransparent(false)
	// 		// .LabelText(CachedGraphNode->RuntimeNode->GetAnimAssetLabel())
	// 		// .LabelTooltipText(CachedGraphNode->RuntimeNode->GetAnimAssetLabelTooltip())
	// 		// .ValueTooltipText(CachedGraphNode->RuntimeNode->GetAnimAssetLabelTooltip())
	// 		.OnGetValueText(FACEOnGetValueText::CreateSP(this, &SThinkGraphNode::GetMontageText))
	// 	];
	//
	// 	/*
	// 	Content->AddSlot()
	// 	       .Padding(RowPadding)
	// 	       .AutoHeight()
	// 	[
	// 		SNew(SThinkGraphNodeRow, CachedGraphNode->RuntimeNode)
	// 		.IsTransparent(false)
	// 		.LabelText(LOCTEXT("PlayRate", "Rate"))
	// 		.LabelTooltipText(LOCTEXT("PlayRateTooltip", "Montage Play Rate"))
	// 		.ValueTooltipText(LOCTEXT("PlayRateValueTooltip", "Montage Play Rate"))
	// 		// .OnGetValueText(FACEOnGetValueText::CreateSP(this, &SHBThinkGraphNode::GetMontagePlayRateText))
	// 	];
	//
	// 	if (CachedGraphNode->RuntimeNode)
	// 	{
	// 		Content->AddSlot()
	// 		       .Padding(RowPadding)
	// 		       .AutoHeight()
	// 		[
	// 			SNew(SThinkGraphNodeRow, CachedGraphNode->RuntimeNode)
	// 			.IsTransparent(false)
	// 			.LabelText(LOCTEXT("StartSection", "Montage Start Section"))
	// 			.LabelTooltipText(LOCTEXT("StartSection", "Montage Start Section"))
	// 			.ValueTooltipText(LOCTEXT("StartSection", "Montage Start Section"))
	// 			.OnGetValueText(FACEOnGetValueText::CreateSP(this, &SHBThinkGraphNode::GetMontageStartSectionName))
	// 			.Visibility(this, &SHBThinkGraphNode::GetMontageStartSectionNameVisibility)
	// 		];
	// 	}
	//
	// 	Content->AddSlot()
	// 	       .Padding(RowPadding)
	// 	       .AutoHeight()
	// 	[
	// 		SNew(SThinkGraphNodeRow, CachedGraphNode->RuntimeNode)
	// 		.IsTransparent(false)
	// 		.LabelText(LOCTEXT("EffectCost", "Effect Cost"))
	// 		.LabelTooltipText(LOCTEXT("EffectCost", "Effect Cost"))
	// 		.ValueTooltipText(LOCTEXT("EffectCost", "Effect Cost"))
	// 		// .OnGetValueText(FACEOnGetValueText::CreateSP(this, &SHBThinkGraphNode::GetGameplayEffectCostText))
	// 		// .Visibility(this, &SHBThinkGraphNode::GetGameplayEffectCostTextVisibility)
	// 	];*/
	//
	// 	if (CachedGraphNode->RuntimeNode->IsHBActionSubclassedInBlueprint())
	// 	{
	// 		FString Classname = GetNameSafe(CachedGraphNode->RuntimeNode->GetClass());
	// 		Classname.RemoveFromEnd("_C");
	//
	// 		Content->AddSlot()
	// 		       .Padding(BottomRowPadding)
	// 		       .AutoHeight()
	// 		[
	// 			SNew(SThinkGraphNodeRow, CachedGraphNode->RuntimeNode)
	// 			.IsTransparent(true)
	// 			// .LabelText(LOCTEXT("NodeClass", "Node Class"))
	// 			.LabelTooltipText(LOCTEXT("NodeClass", "Node Class"))
	// 			.ValueTooltipText(LOCTEXT("NodeClass", "Node Class"))
	// 			.OnGetValueText(FACEOnGetValueText::CreateLambda([Classname]()
	// 			                                                        {
	// 				                                                        return FText::FromString(*Classname);
	// 			                                                        }))
	// 		];
	// 	}
}

//
// EVisibility SHBThinkGraphNode::GetGameplayEffectCostTextVisibility() const
// {
// 	const TSubclassOf<UGameplayEffect> CostGE = CachedGraphNode->RuntimeNode->CostGameplayEffect;
// 	return IsValid(CostGE) ? EVisibility::Visible : EVisibility::Collapsed;
// }
//
// FText SHBThinkGraphNode::GetGameplayEffectCostText() const
// {
// 	const TSubclassOf<UGameplayEffect> CostGE = CachedGraphNode->RuntimeNode->CostGameplayEffect;
// 	return FText::FromString(CostGE ? CostGE->GetName() : TEXT("NONE"));
// }


FText SThinkGraphNode::GetMontageStartSectionName() const
{
	FName StartSectionName = NAME_None;
	return FText::FromName(StartSectionName);
}

EVisibility SThinkGraphNode::GetMontageStartSectionNameVisibility() const
{
	if (!CachedGraphNode || !CachedGraphNode->RuntimeNode)
	{
		return EVisibility::Collapsed;
	}

	const FName StartSectionName;
	return StartSectionName != NAME_None ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SThinkGraphNode::GetErrorText() const
{
	return LOCTEXT("ErrorText", "Test Error Yo");
}


#undef LOCTEXT_NAMESPACE
