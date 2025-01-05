// Fill out your copyright notice in the Description page of Project Settings.


#include "SMemoryBufferEditor.h"

#include "EditorFontGlyphs.h"
#include "SlateOptMacros.h"
#include "SPromptBufferEditor.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "Graph/ThinkGraphEdGraph.h"
#include "Graph/Nodes/ThinkGraphEdNode_Memory.h"
#include "ThinkGraph/TGTypes.h"
#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/ThinkGraphDelegates.h"
#include "Widgets/Images/SSpinningImage.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SMemoryBufferEditor"

// Type Keywords are copied from CrossCompiler::EHlslToken

const TCHAR* MemoryOperators[] =
{
	TEXT("${"),
	TEXT("}")
};

const TCHAR* MemoryKeywords[] =
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
 * Tokenize the text based on Memory tokens
 */
class FMemorySyntaxTokenizer : public ISyntaxTokenizer
{
public:
	/** 
	 * Create a new tokenizer
	 */
	static TSharedRef<FMemorySyntaxTokenizer> Create()
	{
		return MakeShareable(new FMemorySyntaxTokenizer());
	};

	TArray<FString> Keywords;
	TArray<FString> Operators;

	FMemorySyntaxTokenizer()
	{
		// keywords
		for (const auto& Keyword : MemoryKeywords)
		{
			Keywords.Emplace(Keyword);
		}
		for (const auto& Operator : MemoryOperators)
		{
			Operators.Emplace(Operator);
		}
	}

	virtual ~FMemorySyntaxTokenizer()
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


TSharedRef<FMemorySyntaxHighlighterMarshaller> FMemorySyntaxHighlighterMarshaller::Create(
	const FSyntaxTextStyle& InSyntaxTextStyle)
{
	return MakeShareable(new FMemorySyntaxHighlighterMarshaller(CreateTokenizer(), InSyntaxTextStyle));
}

TSharedPtr<ISyntaxTokenizer> FMemorySyntaxHighlighterMarshaller::CreateTokenizer()
{
	return FMemorySyntaxTokenizer::Create();
}

void FMemorySyntaxHighlighterMarshaller::ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout,
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

FTextLayout::FNewLineData FMemorySyntaxHighlighterMarshaller::ProcessTokenizedLine(
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

		FRunInfo RunInfo(TEXT("SyntaxHighlight.Memory.Normal"));
		FTextBlockStyle TextBlockStyle = SyntaxTextStyle.NormalTextStyle;

		const bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
		if (!bIsWhitespace)
		{
			bool bHasMatchedSyntax = false;
			if (Token.Type == ISyntaxTokenizer::ETokenType::Syntax)
			{
				if (ParseState == EParseState::None && TokenText == TEXT("${"))
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
					ParseState = EParseState::Variable;
				}
				else if (ParseState == EParseState::Variable && TokenText == TEXT("}"))
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
					ParseState = EParseState::None;
				}
			}

			// It's possible that we fail to match a syntax token if we're in a state where it isn't parsed
			// In this case, we treat it as a literal token
			if (Token.Type == ISyntaxTokenizer::ETokenType::Literal || !bHasMatchedSyntax)
			{
				if (ParseState == EParseState::LookingForString)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.String");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::LookingForCharacter)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.String");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::LookingForSingleLineComment)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.Comment");
					TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
				}
				else if (ParseState == EParseState::Variable)
				{
					RunInfo.Name = TEXT("SyntaxHighlight.Memory.Variable");
					TextBlockStyle = SyntaxTextStyle.VariableTextStyle;
				}
			}

			TSharedRef<ISlateRun> Run = FSlateTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange);
			Runs.Add(Run);
		}
		else
		{
			RunInfo.Name = TEXT("SyntaxHighlight.Memory.WhiteSpace");
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

FMemorySyntaxHighlighterMarshaller::FMemorySyntaxHighlighterMarshaller(TSharedPtr<ISyntaxTokenizer> InTokenizer,
                                                                       const FSyntaxTextStyle& InSyntaxTextStyle) :
	FSyntaxHighlighterTextLayoutMarshaller(MoveTemp(InTokenizer))
	, SyntaxTextStyle(InSyntaxTextStyle)
{
}


void SMemoryBufferEditor::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = FThinkGraphEditorStyle::Get().GetFontStyle("ThinkGraph.Text.Memory");
	FontInfo.Size = 10;


	auto TextStyle = FPromptSyntaxHighlighterMarshaller::FSyntaxTextStyle(
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Normal"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Variable"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.SearchString"),
		FThinkGraphEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.Prompt.Error"));

	EditedNode = InArgs._Node;
	const TSharedPtr<FPromptSyntaxHighlighterMarshaller> SyntaxHighlighter =
		FPromptSyntaxHighlighterMarshaller::Create(TextStyle);

	const FSearchBoxStyle& SearchBoxStyle = FThinkGraphEditorStyle::Get().GetWidgetStyle<FSearchBoxStyle>(
		TEXT("TextEditor.SearchBoxStyle"));


	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .FillHeight(1.f)
		  .VAlign(VAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			  .VAlign(VAlign_Fill)
			  .HAlign(HAlign_Fill)
			[
				SAssignNew(TextBox, SMultiLineEditableTextBox)
							.Marshaller(SyntaxHighlighter)
							.Font(FontInfo)
							.BackgroundColor(FLinearColor::Black)
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
							.AutoWrapText(true)
							.AllowMultiLine(true)
							.Margin(0.f)
							.Text(this, &SMemoryBufferEditor::GetBufferText)

						.OnTextChanged(this, &SMemoryBufferEditor::OnTextChanged)
			]
			+ SOverlay::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Center)
			[

				SNew(SVerticalBox)
				.Visibility_Lambda([&]
				{
					return bIsGenerating
						       ? EVisibility::Visible
						       : EVisibility::Hidden;
				})
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Center)
				  .HAlign(HAlign_Center)
				  .AutoHeight()
				[
					SNew(SBox)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
                        							.WidthOverride(64.0f)
                        							.HeightOverride(64.0f)
					[

						// Spinning Image while in process
						SNew(SSpinningImage)
					.Period(1.25f)
					.Image(FThinkGraphEditorStyle::Get().GetBrush("ThinkGraph.Icon.Loading"))
					]
				]

				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)

					.Text_Lambda([]
					{
						return FText::FromString(FString::Printf(TEXT("%s %s"),TEXT("GENERATING"),
						                                         *FThinkGraphEditorStyle::GetCommonElipsis()));
					})
				]

			]
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Bottom)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SButton)

					.ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
					.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
					// .ContentPadding(FMargin(6.0, 12.0))
                										.IsEnabled(true)

                										.ToolTipText(LOCTEXT(
					             "BrowseButtonToolTip", "Recalls the memory and fills the buffer"))
                										.OnClicked(this, &SMemoryBufferEditor::HandleMemRecallDebug)
                										.Content()
				[
					SNew(STextBlock)
                                                            							.Font(
						                FAppStyle::Get().GetFontStyle("FontAwesome.14"))
                                                            							.Text(FEditorFontGlyphs::Cogs)
				]
			]
			+ SOverlay::Slot()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.LineHeightPercentage(.5f)
					.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
				.Text_Lambda([&]
				                {
					                if (bIsGenerating)
					                {
						                // Calculate the number of dots based on time (1 to 3 dots cycling every 3 seconds)
						                int32 NumDots = (static_cast<int32>(FPlatformTime::Seconds()) % 3) + 1;
						                FString LoadingText =
							                FString::Printf(TEXT("%.*s"), NumDots, TEXT("..."));
						                return FText::FromString(LoadingText);
					                }
					                else
					                {
						                return FText::FromString(TEXT("Recall Memory"));
					                }
				                })
			]
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
}

FText SMemoryBufferEditor::GetBufferText() const
{
	if (bIsGenerating)
		return FText();


	if (EditedNode->RuntimeNode->InBufferIDS.Num() > 0)
	{
		auto ThinkGraph = CastChecked<UThinkGraph>(
			EditedNode->GetGraph()->GetOuter());
		FString Text = ThinkGraph->GetBuffer(
			EditedNode->RuntimeNode->InBufferIDS[0]).Text.ToString();

		// Calculate how many characters to reveal
		double ElapsedTime = FApp::GetCurrentTime() - StartTime;
		int32 NumVisibleChars = FMath::Clamp(FMath::FloorToInt(ElapsedTime / 0.01), 0, Text.Len());
		return FText::FromString(Text.Left(NumVisibleChars) + (FThinkGraphEditorStyle::GetCommonElipsis()));
	}

	return FText::FromString(TEXT("EMPTY"));
}

void SMemoryBufferEditor::OnTextChanged(const FText& NewText)
{
}

FReply SMemoryBufferEditor::HandleMemRecallDebug()
{
	auto RequestBufferID = EditedNode->RuntimeNode->InBufferIDS;
	FThinkGraphDelegates::OnBufferUpdated.AddRaw(this, &SMemoryBufferEditor::OnMemRecallFinished);

	auto ThinkGraph = CastChecked<UThinkGraph>(EditedNode->GetGraph()->GetOuter());

	if (auto ThinkEdGraph = Cast<UThinkGraphEdGraph>(ThinkGraph->EditorGraph))
	{
		ThinkEdGraph->RebuildGraph();
	}

	for (auto BufferID : EditedNode->RuntimeNode->InBufferIDS)
	{
		ThinkGraph->RequestBufferUpdate(BufferID);
	}

	bIsGenerating= true;

	return FReply::Handled();
}

void SMemoryBufferEditor::OnMemRecallFinished(uint16 ID)
{
	FThinkGraphDelegates::OnBufferUpdated.RemoveAll(this);
	StartTime = FApp::GetCurrentTime();
	bIsGenerating= false;
}

#undef LOCTEXT_NAMESPACE
