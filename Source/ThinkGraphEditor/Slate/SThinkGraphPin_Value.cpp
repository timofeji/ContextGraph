#include "SThinkGraphPin_Value.h"

#include "SLevelOfDetailBranchNode.h"
#include "SPinTypeSelector.h"
#include "ThinkGraphEditorStyle.h"
#include "Components/OverlaySlot.h"

void SThinkGraphPin_Value::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	CachedImg_Pin_ConnectedHovered = FThinkGraphEditorStyle::Get().GetBrush(
		TEXT("ThinkGraph.Pin.Value.ConnectedHovered"));
	CachedImg_Pin_Connected = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.Value.Connected"));
	CachedImg_Pin_DisconnectedHovered = FThinkGraphEditorStyle::Get().GetBrush(
		TEXT("ThinkGraph.Pin.Value.EmptyHovered"));
	CachedImg_Pin_Disconnected = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.Value.Empty"));

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
		MakeAttributeSP(this, &SThinkGraphPin_Value::GetPinIcon),
		MakeAttributeSP(this, &SThinkGraphPin_Value::GetPinColor),
		MakeAttributeSP(this, &SThinkGraphPin_Value::GetSecondaryPinIcon),
		MakeAttributeSP(this, &SThinkGraphPin_Value::GetSecondaryPinColor));
	PinImage = PinWidgetRef;

	PinWidgetRef->SetCursor(
		TAttribute<TOptional<EMouseCursor::Type>>::Create(
			TAttribute<TOptional<EMouseCursor::Type>>::FGetter::CreateRaw(this, &SThinkGraphPin_Value::GetPinCursor)
		)
	);

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
    		.ButtonStyle(FAppStyle::Get(), NAME_NoBorder)
    		.Visibility(this, &SThinkGraphPin_Value::GetPinStatusIconVisibility)
    		.ContentPadding(0)
    		.OnClicked(this, &SThinkGraphPin_Value::ClickedOnPinStatusIcon)
		[

			SNew(SOverlay)
			+ SOverlay::Slot()
			  .VAlign(VAlign_Fill)
			  .HAlign(HAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SThinkGraphPin_Value::GetTypeIcon)
			]
			+ SOverlay::Slot()
			  .VAlign(VAlign_Fill)
			  .HAlign(HAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SThinkGraphPin_Value::GetPinStatusIcon)
			]

		];

	TSharedRef<SWidget> LabelWidget = SNew(STextBlock)
    		.Text(this, &SThinkGraphPin_Value::GetPinLabel)
    		.TextStyle(FThinkGraphEditorStyle::Get(),TEXT("ThinkGraph.Text.Pin"))
    		.Visibility(this, &SThinkGraphPin_Value::GetPinLabelVisibility)
    		.ColorAndOpacity(this, &SThinkGraphPin_Value::GetPinTextColor);

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
				ValueBox->SetEnabled(TAttribute<bool>(this, &SThinkGraphPin_Value::IsEditingEnabled));
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
			  .Padding(0, 0, 5.f, 0)
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
			  .Padding(5.f, 0, 0, 0)
			[
				PinWidgetRef
			];
	}

	// Set up a hover for pins that is tinted the color of the pin.

	auto PinNameLODNode = SNew(SLevelOfDetailBranchNode)
                               				.UseLowDetailSlot(this, &SThinkGraphPin_Value::UseLowDetailPinNames)
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
	                   .BorderImage(this, &SThinkGraphPin_Value::GetPinBorder)
	                   .BorderBackgroundColor(this, &SThinkGraphPin_Value::GetHighlightColor)
	                   .OnMouseButtonDown(this, &SThinkGraphPin_Value::OnPinNameMouseDown)
		[
			SNew(SBorder)
    			.BorderImage(CachedImg_Pin_DiffOutline)
    			.BorderBackgroundColor(this, &SThinkGraphPin_Value::GetPinDiffColor)
			[
				PinNameLODNode
			]
		]
	);

	TSharedPtr<IToolTip> TooltipWidget = SNew(SToolTip)
		.Text(this, &SThinkGraphPin_Value::GetTooltipText);

	SetToolTip(TooltipWidget);
}

const FSlateBrush* SThinkGraphPin_Value::GetTypeIcon() const
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

const FSlateBrush* SThinkGraphPin_Value::GetPinIcon() const
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


// TSharedRef<SWidget> SThinkGraphPin_Value::GetDefaultValueWidget()
// {
// 	return SNew(STextBlock);
// }
//
// const FSlateBrush* SThinkGraphPin_Value::GetPinIcon() const
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
FSlateColor SThinkGraphPin_Value::GetPinColor() const
{
	return IsHovered() ? FLinearColor::White : FLinearColor::White.Desaturate(.4f);
}
