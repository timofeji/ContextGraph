#include "ThinkGraphNode_LLM.h"

#include "HttpModule.h"
#include "ThinkGraph.h"
#include "ThinkGraphDelegates.h"
#include "ThinkGraphNode_Const.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Interfaces/IHttpResponse.h"

#define LOCTEXT_NAMESPACE "ThinkGraphNodeSequence"

UThinkGraphNode_LLM::UThinkGraphNode_LLM()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("ThinkGraphNodeSequence_ContextMenuName", "LLM Node");
#endif
}

void UThinkGraphNode_LLM::Activate(UThinkGraph* ThinkGraph)
{
	if (InBufferIDS.Num() < 1)
	{
		return;
	}
	if(!ThinkGraph)
	{
		return;
	}
	//

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Set up HTTP headers
	HttpRequest->SetURL(Settings.APIEndpoint);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Settings.APIKey));

	Graph = ThinkGraph;

	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (int i = 0; i < InputRoles.Num(); i++)
	{
		FDataBuffer Buffer = ThinkGraph->GetBuffer(InBufferIDS[i]);

		if (!Buffer.Text.IsEmpty())
		{
			// Create the first message object (system message)
			TSharedPtr<FJsonObject> Message = MakeShareable(new FJsonObject());
			Message->SetStringField(TEXT("role"), InputRoles[i].ToLower());
			Message->SetStringField(TEXT("content"), Buffer.Text.ToString());

			MessagesArray.Add(MakeShareable(new FJsonValueObject(Message)));
		}
	}


	// Build JSON payload
	TSharedPtr<FJsonObject> Request = MakeShareable(new FJsonObject());
	Request->SetStringField(TEXT("model"), TEXT("gpt-4"));
	Request->SetArrayField(TEXT("messages"), MessagesArray);
	Request->SetNumberField(TEXT("max_tokens"), NumTokensToGenerate);
	Request->SetNumberField(TEXT("temperature"), Temperature);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(Request.ToSharedRef(), Writer);

	HttpRequest->SetContentAsString(RequestBody);

	// Set up the response callback
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnAPICallback);
	HttpRequest->ProcessRequest();

#if WITH_EDITOR
	bIsGenerating = true;
#endif
}

void UThinkGraphNode_LLM::AddPromptInput(const FString Role)
{
	InputRoles.Add(Role);
}

void UThinkGraphNode_LLM::OnAPICallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseContent = Response->GetContentAsString();

		// Parse the JSON response
		TSharedPtr<FJsonObject> JsonResponse;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
		if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
		{
			// Extract the result
			const TArray<TSharedPtr<FJsonValue>>* Choices;
			if (JsonResponse->TryGetArrayField(TEXT("choices"), Choices) && Choices->Num() > 0)
			{
				const TSharedPtr<FJsonObject>* Message;
				if ((*Choices)[0]->AsObject()->TryGetObjectField(TEXT("message"), Message))
				{
					FString Content;
					if ((*Message)->TryGetStringField(TEXT("content"), Content))
					{
						UE_LOG(LogTemp, Verbose, TEXT("LanguageModel API Response: %s"), *Content);
						
						FDataBuffer& OutputBuffer = Graph->GetBuffer(OutBufferIDS[0]);
						OutputBuffer.Text = FText::FromString(Content);

						FThinkGraphDelegates::OnBufferUpdated.Broadcast(OutBufferIDS[0]);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ThinkGraph: Failed to get a response from LanguageModel API call"));
	}

#if WITH_EDITOR
	bIsGenerating = false;
#endif
}


#undef LOCTEXT_NAMESPACE
