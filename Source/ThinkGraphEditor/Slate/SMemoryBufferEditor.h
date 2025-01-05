// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphBufferEditor.h"
#include "ThinkGraphEditorStyle.h"

#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Framework/Text/TextLayout.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Const.h"
#include "ThinkGraph/Nodes/ThinkGraphNode_Memory.h"
#include "Widgets/Input/SSearchBox.h"


class FMemorySyntaxHighlighterMarshaller : public FSyntaxHighlighterTextLayoutMarshaller
{
public:
	struct FSyntaxTextStyle
	{
		FSyntaxTextStyle(
			const FTextBlockStyle& InNormalTextStyle,
			const FTextBlockStyle& InVariableTextStyle,
			const FTextBlockStyle& InErrorTextStyle
		) :
			NormalTextStyle(InNormalTextStyle),
			VariableTextStyle(InVariableTextStyle),
			ErrorTextStyle(InErrorTextStyle)
		{
		}

		FTextBlockStyle NormalTextStyle;
		FTextBlockStyle VariableTextStyle;
		FTextBlockStyle ErrorTextStyle;
	};

	static THINKGRAPHEDITOR_API TSharedRef<FMemorySyntaxHighlighterMarshaller> Create(
		const FSyntaxTextStyle& InSyntaxTextStyle);

protected:
	enum class EParseState : uint8
	{
		None,
		LookingForString,
		LookingForCharacter,
		LookingForSingleLineComment,
		Variable,
	};

	static THINKGRAPHEDITOR_API TSharedPtr<ISyntaxTokenizer> CreateTokenizer();

	THINKGRAPHEDITOR_API virtual void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout,
	                                              TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines) override;

	THINKGRAPHEDITOR_API virtual FTextLayout::FNewLineData ProcessTokenizedLine(
		const ISyntaxTokenizer::FTokenizedLine& TokenizedLine, const int32& LineNumber, const FString& SourceString,
		EParseState& CurrentParseState);

	THINKGRAPHEDITOR_API FMemorySyntaxHighlighterMarshaller(TSharedPtr<ISyntaxTokenizer> InTokenizer,
	                                                        const FSyntaxTextStyle& InSyntaxTextStyle);

	/** Styles used to display the text */
	FSyntaxTextStyle SyntaxTextStyle;
};


class UThinkGraphEdNode_Memory;
/**
 * 
 */
class THINKGRAPHEDITOR_API SMemoryBufferEditor : public UThinkGraphBufferEditor
{
public:
	friend SMultiLineEditableTextBox;
	SLATE_BEGIN_ARGS(SMemoryBufferEditor)
		{
		}

		SLATE_ARGUMENT(UThinkGraphEdNode_Memory*, Node)
	SLATE_END_ARGS()


	UThinkGraphEdNode_Memory* EditedNode;
	
	double StartTime;
	bool bIsGenerating;


	FReply HandleMemRecallDebug();

	void OnMemRecallFinished(uint16 ID);

	FText GetBufferText() const;
	void OnTextChanged(const FText& NewText);
	
	void Construct(const FArguments& InArgs);
	

private:
	TSharedPtr<SMultiLineEditableTextBox> TextBox;

	



	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	              FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	              bool bParentEnabled) const override
	{
		int32 CurrentLayer = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId,
		                                              InWidgetStyle, bParentEnabled);


		return CurrentLayer + 1;
	}
};
