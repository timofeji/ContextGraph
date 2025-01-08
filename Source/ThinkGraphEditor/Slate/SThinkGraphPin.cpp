#include "SThinkGraphPin.h"

#include "SLevelOfDetailBranchNode.h"
#include "SPinTypeSelector.h"
#include "ThinkGraphEditorStyle.h"
#include "Components/OverlaySlot.h"

void SThinkGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	CachedImg_Pin_ConnectedHovered = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.ConnectedHovered"));
	CachedImg_Pin_Connected = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.Connected"));
	CachedImg_Pin_DisconnectedHovered = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.EmptyHovered"));
	CachedImg_Pin_Disconnected = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.Empty"));

	bShowLabel = true;

	bUsePinColorForText = true;
	this->SetCursor(EMouseCursor::Default);

	SetVisibility(EVisibility::Visible);

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	checkf(
		Schema,
		TEXT("Missing schema for pin: %s with outer: %s of type %s"),
		*(GraphPinObj->GetName()),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetName()) : TEXT("NULL OUTER"),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetClass()->GetName()) : TEXT("NULL OUTER")
	);

	const bool bIsInput = (GetDirection() == EGPD_Input);

	// Create the pin icon widget
	TSharedRef<SWidget> PinWidgetRef = SPinTypeSelector::ConstructPinTypeImage(
		MakeAttributeSP(this, &SThinkGraphPin::GetPinIcon),
		MakeAttributeSP(this, &SThinkGraphPin::GetPinColor),
		MakeAttributeSP(this, &SThinkGraphPin::GetSecondaryPinIcon),
		MakeAttributeSP(this, &SThinkGraphPin::GetSecondaryPinColor));
	PinImage = PinWidgetRef;

	PinWidgetRef->SetCursor(
		TAttribute<TOptional<EMouseCursor::Type>>::Create(
			TAttribute<TOptional<EMouseCursor::Type>>::FGetter::CreateRaw(this, &SThinkGraphPin::GetPinCursor)
		)
	);

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
    		.ButtonStyle(FAppStyle::Get(), NAME_NoBorder)
    		.Visibility(this, &SThinkGraphPin::GetPinStatusIconVisibility)
    		.ContentPadding(0)
    		.OnClicked(this, &SThinkGraphPin::ClickedOnPinStatusIcon)
		[

			SNew(SOverlay)
			+ SOverlay::Slot()
			  .VAlign(VAlign_Fill)
			  .HAlign(HAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SThinkGraphPin::GetTypeIcon)
			]
			+ SOverlay::Slot()
			  .VAlign(VAlign_Fill)
			  .HAlign(HAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SThinkGraphPin::GetPinStatusIcon)
			]

		];

	TSharedRef<SWidget> LabelWidget = SNew(STextBlock)
    		.Text(this, &SThinkGraphPin::GetPinLabel)
    		.TextStyle(FThinkGraphEditorStyle::Get(),TEXT("ThinkGraph.Text.Pin"))
    		.Visibility(this, &SThinkGraphPin::GetPinLabelVisibility)
    		.ColorAndOpacity(this, &SThinkGraphPin::GetPinTextColor);

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	if (!bIsInput)
	{
		LabelAndValue->AddSlot()
		             .VAlign(VAlign_Center)
		[
			PinStatusIndicator
		];

		LabelAndValue->AddSlot()
		             .VAlign(VAlign_Center)
		[
			LabelWidget
		];
	}
	else
	{
		LabelAndValue->AddSlot()
		             .VAlign(VAlign_Center)
		[
			LabelWidget
		];

		ValueWidget = GetDefaultValueWidget();

		if (ValueWidget != SNullWidget::NullWidget)
		{
			TSharedPtr<SBox> ValueBox;
			LabelAndValue->AddSlot()
			             .Padding(bIsInput
				                      ? FMargin(0, 0, 0, 0)
				                      : FMargin(0, 0, 0, 0))
			             .VAlign(VAlign_Center)
			[
				SAssignNew(ValueBox, SBox)
				.Padding(0.0f)
				[
					ValueWidget.ToSharedRef()
				]
			];

			if (!DoesWidgetHandleSettingEditingEnabled())
			{
				ValueBox->SetEnabled(TAttribute<bool>(this, &SThinkGraphPin::IsEditingEnabled));
			}
		}

		LabelAndValue->AddSlot()
		             .VAlign(VAlign_Center)
		[
			PinStatusIndicator
		];
	}

	TSharedPtr<SHorizontalBox> PinContent;
	if (bIsInput)
	{
		// Input pin
		FullPinHorizontalRowWidget = PinContent =
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(VAlign_Center)
			  .Padding(0, 0, 0.f, 0)
			[
				PinWidgetRef
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(VAlign_Center)
			[
				LabelAndValue.ToSharedRef()
			];
	}
	else
	{
		// Output pin
		FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(VAlign_Center)
			[
				LabelAndValue.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(VAlign_Center)
			  .Padding(0.f, 0, 0, 0)
			[
				PinWidgetRef
			];
	}

	// Set up a hover for pins that is tinted the color of the pin.

	auto PinNameLODNode = SNew(SLevelOfDetailBranchNode)
                               				.UseLowDetailSlot(this, &SThinkGraphPin::UseLowDetailPinNames)
                               				.LowDetail()
		[
			//@TODO: Try creating a pin-colored line replacement that doesn't measure text / call delegates but still renders
			PinWidgetRef
		]
		.HighDetail()
		[
			PinContent.ToSharedRef()
		];

	SBorder::Construct(SBorder::FArguments()
	                   .Padding(FMargin(0.f))
	                   .BorderImage(this, &SThinkGraphPin::GetPinBorder)
	                   .BorderBackgroundColor(this, &SThinkGraphPin::GetHighlightColor)
	                   .OnMouseButtonDown(this, &SThinkGraphPin::OnPinNameMouseDown)
		[
			SNew(SBorder)
				.Padding(FMargin(1.f))
    			.BorderImage(CachedImg_Pin_DiffOutline)
    			.BorderBackgroundColor(this, &SThinkGraphPin::GetPinDiffColor)
			[
				PinNameLODNode
			]
		]
	);

	TSharedPtr<IToolTip> TooltipWidget = SNew(SToolTip)
		.Text(this, &SThinkGraphPin::GetTooltipText);

	SetToolTip(TooltipWidget);
}

const FSlateBrush* SThinkGraphPin::GetTypeIcon() const
{
	const FSlateBrush* Brush = nullptr;

	UEdGraphPin* GraphPin = GetPinObj();
	if (GraphPin && !GraphPin->IsPendingKill())
	{
		if (UEdGraphNode* GraphNode = GraphPin->GetOwningNodeUnchecked())
		{
			auto PinName = GraphNode->GetPinDisplayName(GraphPin);
			if (PinName.ToString().Contains(TEXT("Prompt")))
			{
				Brush = FSlateIcon(FThinkGraphEditorStyle::GetStyleSetName(), "ThinkGraph.Icon.Prompt").
					GetOptionalIcon();
			}
		}
	}

	return Brush;
}

const FSlateBrush* SThinkGraphPin::GetPinIcon() const
{
	const FSlateBrush* Brush = CachedImg_Pin_Disconnected;

	if (IsConnected())
	{
		Brush = IsHovered() ? CachedImg_Pin_ConnectedHovered : CachedImg_Pin_Connected;
	}
	else
	{
		Brush = IsHovered() ? CachedImg_Pin_DisconnectedHovered : CachedImg_Pin_Disconnected;
	}

	return Brush;
}


// TSharedRef<SWidget> SThinkGraphPin::GetDefaultValueWidget()
// {
// 	return SNew(STextBlock);
// }
//
// const FSlateBrush* SThinkGraphPin::GetPinIcon() const
// {
// 	FString StyleName = IsConnected()
// 		                    ? "ThinkGraph.Pin.Connected"
// 		                    : "ThinkGraph.Pin.Empty";
//
// 	StyleName.Append(IsHovered() ? "Hovered" : "");
//
// 	return FThinkGraphEditorStyle::Get().GetBrush(FName(*StyleName));
// }
//
FSlateColor SThinkGraphPin::GetPinColor() const
{
	return IsHovered() ? FLinearColor::White : FLinearColor::White.Desaturate(.4f);
}
