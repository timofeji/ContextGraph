// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SThinkGraphPin.h"


class SThinkGraphPin_Value : public SThinkGraphPin
{
public:
	SLATE_BEGIN_ARGS(SThinkGraphPin_Value){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
	// virtual const FSlateBrush* GetPinIcon() const override;

protected:
	// Begin SGraphPin interface
	// virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	const FSlateBrush* GetTypeIcon() const;
	const FSlateBrush* GetPinIcon() const;
	FSlateColor GetPinColor() const override;

	
	mutable const FSlateBrush* CachedImg_Pin_ConnectedHovered;
	mutable const FSlateBrush* CachedImg_Pin_DisconnectedHovered;
};
