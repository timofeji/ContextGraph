// Fill out your copyright notice in the Description page of Project Settings.


#include "SPromptBufferEditor.h"

#include "SlateOptMacros.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateTextRun.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


// Type Keywords are copied from CrossCompiler::EHlslToken

const TCHAR* PromptOperators[] =
{
	TEXT("${"),
	TEXT("}")
};

const TCHAR* HlslKeywords[] =
{
	TEXT("while"),
	TEXT("volatile"),
	TEXT("void"),
	TEXT("vector"),
	TEXT("unorm"),
	TEXT("uniform"),
	TEXT("uint4x4"),
	TEXT("uint4x3"),
	TEXT("uint4x2"),
	TEXT("uint4x1"),
	TEXT("uint4"),
	TEXT("uint3x4"),
	TEXT("uint3x3"),
	TEXT("uint3x2"),
	TEXT("uint3x1"),
	TEXT("uint3"),
	TEXT("uint2x4"),
	TEXT("uint2x3"),
	TEXT("uint2x2"),
	TEXT("uint2x1"),
	TEXT("uint2"),
	TEXT("uint1x4"),
	TEXT("uint1x3"),
	TEXT("uint1x2"),
	TEXT("uint1x1"),
	TEXT("uint1"),
	TEXT("uint"),
	TEXT("true"),
	TEXT("switch"),
	TEXT("struct"),
	TEXT("static"),
	TEXT("snorm"),
	TEXT("shared"),
	TEXT("row_major"),
	TEXT("return"),
	TEXT("register"),
	TEXT("precise"),
	TEXT("packoffset"),
	TEXT("numthreads"),
	TEXT("nointerpolation"),
	TEXT("namespace"),
	TEXT("matrix"),
	TEXT("int4x4"),
	TEXT("int4x3"),
	TEXT("int4x2"),
	TEXT("int4x1"),
	TEXT("int4"),
	TEXT("int3x4"),
	TEXT("int3x3"),
	TEXT("int3x2"),
	TEXT("int3x1"),
	TEXT("int3"),
	TEXT("int2x4"),
	TEXT("int2x3"),
	TEXT("int2x2"),
	TEXT("int2x1"),
	TEXT("int2"),
	TEXT("int1x4"),
	TEXT("int1x3"),
	TEXT("int1x2"),
	TEXT("int1x1"),
	TEXT("int1"),
	TEXT("int"),
	TEXT("if"),
	TEXT("half4x4"),
	TEXT("half4x3"),
	TEXT("half4x2"),
	TEXT("half4x1"),
	TEXT("half4"),
	TEXT("half3x4"),
	TEXT("half3x3"),
	TEXT("half3x2"),
	TEXT("half3x1"),
	TEXT("half3"),
	TEXT("half2x4"),
	TEXT("half2x3"),
	TEXT("half2x2"),
	TEXT("half2x1"),
	TEXT("half2"),
	TEXT("half1x4"),
	TEXT("half1x3"),
	TEXT("half1x2"),
	TEXT("half1x1"),
	TEXT("half1"),
	TEXT("half"),
	TEXT("groupshared"),
	TEXT("goto"),
	TEXT("for"),
	TEXT("float4x4"),
	TEXT("float4x3"),
	TEXT("float4x2"),
	TEXT("float4x1"),
	TEXT("float4"),
	TEXT("float3x4"),
	TEXT("float3x3"),
	TEXT("float3x2"),
	TEXT("float3x1"),
	TEXT("float3"),
	TEXT("float2x4"),
	TEXT("float2x3"),
	TEXT("float2x2"),
	TEXT("float2x1"),
	TEXT("float2"),
	TEXT("float1x4"),
	TEXT("float1x3"),
	TEXT("float1x2"),
	TEXT("float1x1"),
	TEXT("float1"),
	TEXT("float"),
	TEXT("false"),
	TEXT("extern"),
	TEXT("export"),
	TEXT("enum"),
	TEXT("else"),
	TEXT("dword"),
	TEXT("double"),
	TEXT("do"),
	TEXT("default"),
	TEXT("continue"),
	TEXT("const"),
	TEXT("column_major"),
	TEXT("case"),
	TEXT("break"),
	TEXT("bool4x4"),
	TEXT("bool4x3"),
	TEXT("bool4x2"),
	TEXT("bool4x1"),
	TEXT("bool4"),
	TEXT("bool3x4"),
	TEXT("bool3x3"),
	TEXT("bool3x2"),
	TEXT("bool3x1"),
	TEXT("bool3"),
	TEXT("bool2x4"),
	TEXT("bool2x3"),
	TEXT("bool2x2"),
	TEXT("bool2x1"),
	TEXT("bool2"),
	TEXT("bool1x4"),
	TEXT("bool1x3"),
	TEXT("bool1x2"),
	TEXT("bool1x1"),
	TEXT("bool1"),
	TEXT("bool"),
	TEXT("Buffer"),
	TEXT("in"),
	TEXT("out"),
	TEXT("inout"),
};

/**
 * Tokenize the text based on Prompt tokens
 */
class FPromptSyntaxTokenizer : public ISyntaxTokenizer
{
public:
	/** 
	 * Create a new tokenizer
	 */
	static TSharedRef<FPromptSyntaxTokenizer> Create()
	{
		return MakeShareable(new FPromptSyntaxTokenizer());
	};

	TArray<FString> Keywords;
	TArray<FString> Operators;
	FString SearchString;

	FPromptSyntaxTokenizer()
	{
		// keywords
		for (const auto& Keyword : HlslKeywords)
		{
			Keywords.Emplace(Keyword);
		}
		for (const auto& Operator : PromptOperators)
		{
			Operators.Emplace(Operator);
		}
	}

	virtual ~FPromptSyntaxTokenizer()
	{
	};

	virtual void Process(TArray<FTokenizedLine>& OutTokenizedLines, const FString& Input) override
	{
#if UE_ENABLE_ICU
		TArray<FTextRange> LineRanges;
		FTextRange::CalculateLineRangesFromString(Input, LineRanges);
		TokenizeLineRanges(Input, LineRanges, OutTokenizedLines);
#else
		FTokenizedLine FakeTokenizedLine;
		FakeTokenizedLine.Range = FTextRange(0, Input.Len());
		FakeTokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, FakeTokenizedLine.Range));
		OutTokenizedLines.Add(FakeTokenizedLine);
#endif
	};

private:
	void TokenizeLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges,
	                        TArray<FTokenizedLine>& OutTokenizedLines)
	{
		// Tokenize line ranges
		for (const FTextRange& LineRange : LineRanges)
		{
			FTokenizedLine TokenizedLine;
			TokenizedLine.Range = LineRange;

			if (TokenizedLine.Range.IsEmpty())
			{
				TokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, TokenizedLine.Range));
			}
			else
			{
				int32 CurrentOffset = LineRange.BeginIndex;

				while (CurrentOffset < LineRange.EndIndex)
				{
					const TCHAR* CurrentString = &Input[CurrentOffset];
					const TCHAR CurrentChar = Input[CurrentOffset];

					bool bHasMatchedSyntax = false;


					for (const FString& Operator : Operators)
					{
						if (FCString::Strncmp(CurrentString, *Operator, Operator.Len()) == 0)
						{
							const int32 SyntaxTokenEnd = CurrentOffset + Operator.Len();
							TokenizedLine.Tokens.Emplace(FToken(ETokenType::Syntax,
							                                    FTextRange(CurrentOffset, SyntaxTokenEnd)));

							check(SyntaxTokenEnd <= LineRange.EndIndex);

							bHasMatchedSyntax = true;
							CurrentOffset = SyntaxTokenEnd;
							break;
						}
					}

					if (bHasMatchedSyntax)
					{
						continue;
					}

					int32 PeekOffset = CurrentOffset + 1;

					if (CurrentChar == TEXT('#'))
					{
						// Match PreProcessorKeywords
						// They only contain letters
						while (PeekOffset < LineRange.EndIndex)
						{
							const TCHAR PeekChar = Input[PeekOffset];

							if (!TChar<TCHAR>::IsAlpha(PeekChar))
							{
								break;
							}

							PeekOffset++;
						}
					}
					else if (TChar<TCHAR>::IsAlpha(CurrentChar))
					{
						// Match Identifiers,
						// They start with a letter and contain
						// letters or numbers
						while (PeekOffset < LineRange.EndIndex)
						{
							const TCHAR PeekChar = Input[PeekOffset];

							if (!TChar<TCHAR>::IsIdentifier(PeekChar))
							{
								break;
							}

							PeekOffset++;
						}
					}

					const int32 CurrentStringLength = PeekOffset - CurrentOffset;

					// Check if it is an reserved keyword
					for (const FString& Keyword : Keywords)
					{
						if (FCString::Strncmp(CurrentString, *Keyword,
						                      FMath::Max(CurrentStringLength, Keyword.Len())) == 0)
						{
							const int32 SyntaxTokenEnd = CurrentOffset + CurrentStringLength;
							TokenizedLine.Tokens.Emplace(FToken(ETokenType::Syntax,
							                                    FTextRange(CurrentOffset, SyntaxTokenEnd)));

							check(SyntaxTokenEnd <= LineRange.EndIndex);

							bHasMatchedSyntax = true;
							CurrentOffset = SyntaxTokenEnd;
							break;
						}
					}

					if (bHasMatchedSyntax)
					{
						continue;
					}


					if (!SearchString.IsEmpty())
					{
						if (FCString::Strncmp(CurrentString, *SearchString, SearchString.Len()) == 0)
						{
							const int32 SyntaxTokenEnd = CurrentOffset + SearchString.Len();
							TokenizedLine.Tokens.Emplace(FToken(ETokenType::Syntax,
							                                    FTextRange(CurrentOffset, SyntaxTokenEnd)));

							check(SyntaxTokenEnd <= LineRange.EndIndex);

							bHasMatchedSyntax = true;
							CurrentOffset = SyntaxTokenEnd;
						}

						if (bHasMatchedSyntax)
						{
							continue;
						}
					}

					// If none matched, consume the character(s) as text
					const int32 TextTokenEnd = CurrentOffset + CurrentStringLength;
					TokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, FTextRange(CurrentOffset, TextTokenEnd)));
					CurrentOffset = TextTokenEnd;
				}
			}

			OutTokenizedLines.Add(TokenizedLine);
		}
	};
};


class FWhiteSpaceTextRun : public FSlateTextRun
{
public:
	static TSharedRef<FWhiteSpaceTextRun> Create(
		const FRunInfo& InRunInfo,
		const TSharedRef<const FString>& InText,
		const FTextBlockStyle& Style,
		const FTextRange& InRange,
		int32 NumSpacesPerTab)
	{
		return MakeShareable(new FWhiteSpaceTextRun(InRunInfo, InText, Style, InRange, NumSpacesPerTab));
	}

public:
	virtual FVector2D Measure(
		int32 StartIndex,
		int32 EndIndex,
		float Scale,
		const FRunTextContext& TextContext
	) const override
	{
		const FVector2D ShadowOffsetToApply(
			(EndIndex == Range.EndIndex) ? FMath::Abs(Style.ShadowOffset.X * Scale) : 0.0f,
			FMath::Abs(Style.ShadowOffset.Y * Scale));

		if (EndIndex - StartIndex == 0)
		{
			return FVector2D(ShadowOffsetToApply.X * Scale, GetMaxHeight(Scale));
		}

		// count tabs
		int32 TabCount = 0;
		for (int32 Index = StartIndex; Index < EndIndex; Index++)
		{
			if ((*Text)[Index] == TEXT('\t'))
			{
				TabCount++;
			}
		}

		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->
			GetFontMeasureService();
		FVector2D Size = FontMeasure->Measure(**Text, StartIndex, EndIndex, Style.Font, true, Scale) +
			ShadowOffsetToApply;

		Size.X -= TabWidth * TabCount * Scale;
		Size.X += SpaceWidth * TabCount * NumSpacesPerTab * Scale;

		return Size;
	}

protected:
	FWhiteSpaceTextRun(
		const FRunInfo& InRunInfo,
		const TSharedRef<const FString>& InText,
		const FTextBlockStyle& InStyle,
		const FTextRange& InRange,
		int32 InNumSpacesPerTab) :
		FSlateTextRun(InRunInfo, InText, InStyle, InRange),
		NumSpacesPerTab(InNumSpacesPerTab)
	{
		// measure tab width
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->
			GetFontMeasureService();
		TabWidth = FontMeasure->Measure(TEXT("\t"), 0, 1, Style.Font, true, 1.0f).X;
		SpaceWidth = FontMeasure->Measure(TEXT(" "), 0, 1, Style.Font, true, 1.0f).X;
	}

private:
	int32 NumSpacesPerTab;

	double TabWidth;

	double SpaceWidth;
};


TSharedRef<FPromptSyntaxHighlighterMarshaller> FPromptSyntaxHighlighterMarshaller::Create(
	const FSyntaxTextStyle& InSyntaxTextStyle)
{
	return MakeShareable(new FPromptSyntaxHighlighterMarshaller(CreateTokenizer(), InSyntaxTextStyle));
}

void FPromptSyntaxHighlighterMarshaller::SetSearchText(const FText& Text)
{
	if (PromptTokenizer.IsValid())
	{
		PromptTokenizer->SearchString = Text.ToString();
	}
}

TSharedPtr<FPromptSyntaxTokenizer> FPromptSyntaxHighlighterMarshaller::CreateTokenizer()
{
	return FPromptSyntaxTokenizer::Create();
}

void FPromptSyntaxHighlighterMarshaller::ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout,
                                                     TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines)
{
	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(TokenizedLines.Num());

	// Parse the tokens, generating the styled runs for each line
	int32 LineNo = 0;
	EParseState ParseState = EParseState::None;
	for (const ISyntaxTokenizer::FTokenizedLine& TokenizedLine : TokenizedLines)
	{
		LinesToAdd.Add(ProcessTokenizedLine(TokenizedLine, LineNo, SourceString, ParseState));
		LineNo++;
	}

	TargetTextLayout.AddLines(LinesToAdd);
}

FTextLayout::FNewLineData FPromptSyntaxHighlighterMarshaller::ProcessTokenizedLine(
	const ISyntaxTokenizer::FTokenizedLine& TokenizedLine, const int32& LineNumber, const FString& SourceString,
	EParseState& ParseState)
{
	TSharedRef<FString> ModelString = MakeShareable(new FString());
	TArray<TSharedRef<IRun>> Runs;

	for (const ISyntaxTokenizer::FToken& Token : TokenizedLine.Tokens)
	{
		const FString TokenText = SourceString.Mid(Token.Range.BeginIndex, Token.Range.Len());

		const FTextRange ModelRange(ModelString->Len(), ModelString->Len() + TokenText.Len());
		ModelString->Append(TokenText);

		FRunInfo RunInfo(TEXT("SyntaxHighlight.Prompt.Normal"));
		FTextBlockStyle TextBlockStyle = SyntaxTextStyle.NormalTextStyle;

		const bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
		if (!bIsWhitespace)
		{
			bool bHasMatchedSyntax = false;

			if (Token.Type == ISyntaxTokenizer::ETokenType::Syntax)
			{
				if (ParseState == EParseState::None && TokenText == TEXT("${"))
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
					ParseState = EParseState::Variable;
				}
				else if (ParseState == EParseState::Variable && TokenText == TEXT("}"))
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
					ParseState = EParseState::None;
				}
				else
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.SearchString");
					TextBlockStyle = SyntaxTextStyle.SearchStringTextStyle;
					ParseState = EParseState::None;
				}
			}


			// It's possible that we fail to match a syntax token if we're in a state where it isn't parsed
			// In this case, we treat it as a literal token
			if (Token.Type == ISyntaxTokenizer::ETokenType::Literal || !bHasMatchedSyntax)
			{
				if (ParseState == EParseState::LookingForString)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.String");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::LookingForCharacter)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.String");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::LookingForSingleLineComment)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.Comment");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::Variable)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Prompt.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
				}
			}

			TSharedRef<ISlateRun> Run = FSlateTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange);
			Runs.Add(Run);
		}
		else
		{
			RunInfo.Name = TEXT("SyntaxHighlight.Prompt.WhiteSpace");
			TSharedRef<ISlateRun> Run = FWhiteSpaceTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, 4);
			Runs.Add(Run);
		}
	}

	if (ParseState != EParseState::Variable)
	{
		ParseState = EParseState::None;
	}
	return FTextLayout::FNewLineData(MoveTemp(ModelString), MoveTemp(Runs));
}

FPromptSyntaxHighlighterMarshaller::FPromptSyntaxHighlighterMarshaller(TSharedPtr<FPromptSyntaxTokenizer> InTokenizer,
                                                                       const FSyntaxTextStyle& InSyntaxTextStyle) :
	FSyntaxHighlighterTextLayoutMarshaller(MoveTemp(InTokenizer))
	, SyntaxTextStyle(InSyntaxTextStyle)
{
	if (InTokenizer)
	{
		PromptTokenizer = InTokenizer;
		Tokenizer = InTokenizer;
	}
}

void SPromptBufferEditor::OnBufferSearchChanged(const FText& Text)
{
	TextBox->SetSearchText(Text);
}

void SPromptBufferEditor::OnBufferSearchCommitted(const FText& Text, ETextCommit::Type Arg)
{
	TextBox->AdvanceSearch();
}

void SPromptBufferEditor::OnBufferSearchSaved(const FText& Text)
{
}

void SPromptBufferEditor::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = FThinkGraphEditorStyle::Get().GetFontStyle("ThinkGraph.Text.Prompt");
	FontInfo.Size = 10;


	auto PromptSyntaxStyle = FPromptSyntaxHighlighterMarshaller::FSyntaxTextStyle(
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Normal"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Variable"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.SearchString"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Error"));


	ViewedEdNodeConst = InArgs._Node;
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
							.Text_Lambda([&] { return FText::FromString(ViewedEdNodeConst->Text); })
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

void SPromptBufferEditor::Find()
{
	if (SearchBox)
	{
		FSlateApplication::Get().SetKeyboardFocus(SearchBox, EFocusCause::SetDirectly);
	}
}

void SPromptBufferEditor::OnTextChanged(const FText& NewText)
{
	ViewedEdNodeConst->Text = NewText.ToString();
	ViewedEdNodeConst->OnDataUpdated();
	
	UpdateTextHighlights();
}
