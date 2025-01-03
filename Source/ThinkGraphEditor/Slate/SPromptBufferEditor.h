// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphBufferEditor.h"
#include "ThinkGraphEditorStyle.h"

#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Framework/Text/TextLayout.h"
#include "Graph/Nodes/ThinkGraphEdNode_Const.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SPromptBufferEditor"

class FPromptSyntaxTokenizer;

class FPromptSyntaxHighlighterMarshaller : public FSyntaxHighlighterTextLayoutMarshaller
{
public:
	struct FSyntaxTextStyle
	{
		FSyntaxTextStyle(
			const FTextBlockStyle& InNormalTextStyle,
			const FTextBlockStyle& InVariableTextStyle,
			const FTextBlockStyle& InSearchTextStyle,
			const FTextBlockStyle& InErrorTextStyle
		) :
			NormalTextStyle(InNormalTextStyle),
			VariableTextStyle(InVariableTextStyle),
			SearchStringTextStyle(InSearchTextStyle),
			ErrorTextStyle(InErrorTextStyle)
		{
		}

		FTextBlockStyle NormalTextStyle;
		FTextBlockStyle VariableTextStyle;
		FTextBlockStyle SearchStringTextStyle;
		FTextBlockStyle ErrorTextStyle;
	};

	static THINKGRAPHEDITOR_API TSharedRef<FPromptSyntaxHighlighterMarshaller> Create(
		const FSyntaxTextStyle& InSyntaxTextStyle);

	void SetSearchText(const FText& Text);

protected:
	enum class EParseState : uint8
	{
		None,
		LookingForString,
		LookingForCharacter,
		LookingForSingleLineComment,
		Variable,
	};

	static THINKGRAPHEDITOR_API TSharedPtr<FPromptSyntaxTokenizer> CreateTokenizer();

	THINKGRAPHEDITOR_API virtual void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout,
	                                              TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines) override;

	THINKGRAPHEDITOR_API virtual FTextLayout::FNewLineData ProcessTokenizedLine(
		const ISyntaxTokenizer::FTokenizedLine& TokenizedLine, const int32& LineNumber, const FString& SourceString,
		EParseState& CurrentParseState);


	THINKGRAPHEDITOR_API FPromptSyntaxHighlighterMarshaller(TSharedPtr<FPromptSyntaxTokenizer> InTokenizer,
	                                                        const FSyntaxTextStyle& InSyntaxTextStyle);

	/** Styles used to display the text */
	FSyntaxTextStyle SyntaxTextStyle;
	TSharedPtr<FPromptSyntaxTokenizer> PromptTokenizer;
};

/**
 * 
 */
class THINKGRAPHEDITOR_API SPromptBufferEditor : public UThinkGraphBufferEditor
{
public:
	friend SMultiLineEditableTextBox;
	SLATE_BEGIN_ARGS(SPromptBufferEditor)
		{
		}

		SLATE_ARGUMENT(UThinkGraphEdNode_Const*, Node)
	SLATE_END_ARGS()

	TSharedPtr<FPromptSyntaxHighlighterMarshaller> SyntaxHighlighter;
	UThinkGraphEdNode_Const* EditedEdNode;

	void OnBufferSearchChanged(const FText& Text);

	void OnBufferSearchCommitted(const FText& Text, ETextCommit::Type Arg);

	void OnBufferSearchSaved(const FText& Text);

	void Construct(const FArguments& InArgs)
	{
		FSlateFontInfo FontInfo = FThinkGraphEditorStyle::Get().GetFontStyle("ThinkGraph.Text.Prompt");
		FontInfo.Size = 10;


		auto PromptSyntaxStyle = FPromptSyntaxHighlighterMarshaller::FSyntaxTextStyle(
			FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Normal"),
			FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Variable"),
			FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.SearchString"),
			FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Error"));


		EditedEdNode = InArgs._Node;
		SyntaxHighlighter =
			FPromptSyntaxHighlighterMarshaller::Create(PromptSyntaxStyle);


		const FSearchBoxStyle& SearchBoxStyle = FThinkGraphEditorStyle::Get().GetWidgetStyle<FSearchBoxStyle>(
			TEXT("TextEditor.SearchBoxStyle"));

		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .FillHeight(1.f)
			  .VAlign(VAlign_Fill)
			[
				SAssignNew(TextBox, SMultiLineEditableTextBox)
							.Marshaller(SyntaxHighlighter)
							.Font(FontInfo)
							.BackgroundColor(FLinearColor::Black)
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
							.AutoWrapText(true)
							.AllowMultiLine(true)
							.Margin(0.f)
							.Text_Lambda([&] { return EditedEdNode->Prompt; })
							.OnTextChanged(this, &SPromptBufferEditor::OnTextChanged)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .HAlign(HAlign_Fill)
			  .VAlign(VAlign_Bottom)
			[

				// SNew(SScaleBox)
				// .Stretch(EStretch::ScaleToFit)
				// [
				SAssignNew(SearchBox, SSearchBox)
				.HintText(NSLOCTEXT("SearchBox", "HelpHint", "Search For Text"))
				.Style(&SearchBoxStyle)
				.OnTextChanged(this, &SPromptBufferEditor::OnBufferSearchChanged)
				.OnTextCommitted(this, &SPromptBufferEditor::OnBufferSearchCommitted)
				// .SearchResultData(InArgs._SearchResultData)
				.SelectAllTextWhenFocused(true)
				// .OnSearch(InArgs._OnResultNavigationButtonClicked);
				// ]
			]
			// + SVerticalBox::Slot()
			// .FillHeight(1.0f)
			// [
			// 	// SAssignNew(SearchBox, SFilterSearchBox)
			//  //             			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("SequencerFilterSearch")))
			//  //             			.DelayChangeNotificationsWhileTyping(true)
			//  //             			.ShowSearchHistory(true)
			//  //             			.OnTextChanged(this, &SPromptBufferEditor::OnBufferSearchChanged)
			//  //             			.OnTextCommitted(this, &SPromptBufferEditor::OnBufferSearchCommitted)
			//  //             			.OnSaveSearchClicked(this, &SPromptBufferEditor::OnBufferSearchSaved)
			// ]
		];

		UpdateTextHighlights();
	}

	void Find() override;

private:
	TSharedPtr<SMultiLineEditableTextBox> TextBox;
	TSharedPtr<SSearchBox> SearchBox;
	TArray<FSlateRect> HighlightBoxes;

	void OnTextChanged(const FText& NewText)
	{
		EditedEdNode->Prompt = NewText;
		UpdateTextHighlights();
	}


	void UpdateTextHighlights()
	{
		HighlightBoxes.Empty();
	}

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	              FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	              bool bParentEnabled) const override
	{
		int32 CurrentLayer = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId,
		                                              InWidgetStyle, bParentEnabled);

		for (const FSlateRect& HighlightBox : HighlightBoxes)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				CurrentLayer - 1,
				AllottedGeometry.ToPaintGeometry(
					FVector2D(0.f, 4.f),
					FVector2D(40.f, 40.f)
				),
				FCoreStyle::Get().GetBrush("WhiteBrush"),
				ESlateDrawEffect::None,
				FLinearColor::Red
			);
		}

		return CurrentLayer + 1;
	}
};

#undef LOCTEXT_NAMESPACE
