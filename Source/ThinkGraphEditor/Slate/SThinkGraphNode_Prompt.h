// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "SThinkGraphNode.h"


class UThinkGraphEdNode_Const ;

class SThinkGraphNode_Prompt : public SThinkGraphNode
{
public:
	SLATE_BEGIN_ARGS(SThinkGraphNode_Prompt){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UThinkGraphEdNode_Const* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	// virtual void UpdateGraphNode() override;
	// virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	// virtual void CreatePinWidgets() override;
	
	// End of SGraphNode interface

	
	float PinSize = 5.f;
	float PinPadding = 5.f;

protected:
	FSlateColor GetBorderBackgroundColor() const;
	FText GetPreviewCornerText() const;
};
