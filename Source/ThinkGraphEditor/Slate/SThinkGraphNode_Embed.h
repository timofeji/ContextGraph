// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "SThinkGraphNode.h"


class UThinkGraphEdNode_Embed ;

class SThinkGraphNode_Embed : public SThinkGraphNode
{
public:
	SLATE_BEGIN_ARGS(SThinkGraphNode_Embed){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UThinkGraphEdNode_Embed* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddValuePin(const TSharedRef<SGraphPin>& PinToAdd);
	virtual void CreatePinWidgets() override;
	// virtual void UpdateGraphNode() override;

	
	TSharedPtr<SVerticalBox> ValuesBox;
	
	
	TArray< TSharedRef<SGraphPin> > ValuePins;
	
	// End of SGraphNode interface

	
	float PinSize = 5.f;
	float PinPadding = 5.f;

protected:
	FSlateColor GetBorderBackgroundColor() const;
	FText GetPreviewCornerText() const;
};
