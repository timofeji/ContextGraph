#include "ThinkGraphNode_Embed.h"

#include "TGTypes.h"
#include "ThinkGraph.h"
#include "ThinkGraphLog.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNode_Embed"

UThinkGraphNode_Embed::UThinkGraphNode_Embed()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "Embed Buffer");
#endif
}

void UThinkGraphNode_Embed::Activate(UThinkGraph* ThinkGraph)
{
	Super::Activate(ThinkGraph);

	if(InBufferIDS.IsEmpty())
	{
		TG_ERROR(Error,TEXT("%s: Activated with no input buffers."),
        		       *GetName());
		return;
	}

	FDataBuffer& PromptBuffer = ThinkGraph->GetBuffer(InBufferIDS[0]);

	const FString PromptStr = PromptBuffer.Text.ToString();
	const int32 StrLength = PromptStr.Len();
	
	FString OutStr;
	
	int32 i = 0;
	while( i < StrLength)
	{
		// Check for the start of a var token
		if (PromptStr.Mid(i, 2) == TEXT("${"))
		{
			i += 2; //@TODO ~Tim: we can maybe do a signle pass and just store the position of begin and end tokens
			int32 EndIndex = PromptStr.Find(TEXT("}"), ESearchCase::IgnoreCase, ESearchDir::FromStart, i);

			// If closing brace found
			if (EndIndex != INDEX_NONE)
			{
				FString VarName = PromptStr.Mid(i, EndIndex -  i);
				int32 InVarIndex = INDEX_NONE;
				VarKeys.Find(VarName, InVarIndex);
				if (InVarIndex != INDEX_NONE && InBufferIDS.IsValidIndex(InVarIndex + 1))
				{
					FDataBuffer& VarBuffer = ThinkGraph->GetBuffer(InBufferIDS[InVarIndex+1]);
					OutStr += VarBuffer.Text.ToString();
				}
				i = EndIndex + 1;
			}
			else
			{
				// No closing brace found, treat as normal text
				OutStr += TEXT("${");
			}
		}
		else
		{
			// Append literals
			OutStr += PromptStr[i];
			++i;
		}
	}

	FDataBuffer& OutBuffer = ThinkGraph->GetBuffer(OutBufferIDS[0]);
	OutBuffer.Update(FText::FromString(OutStr));
}
#undef LOCTEXT_NAMESPACE
