#include "ThinkGraphNode_LLM.h"

#include "HttpModule.h"
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

void UThinkGraphNode_LLM::Activate()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// Set up HTTP headers
	HttpRequest->SetURL(Settings.APIEndpoint);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	// HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *APIKey));

	TSharedPtr<FJsonObject> Msgs = MakeShareable(new FJsonObject());
	Msgs->SetStringField(TEXT("role"), TEXT("user"));
	Msgs->SetStringField(TEXT("content"), TEXT("What is your name?"));

	// Build JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("prompt"), TEXT("What is your name?"));
	// JsonObject->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	// JsonObject->SetArrayField(TEXT("messages"), TArray<TSharedPtr<FJsonValue>>{
	// 	                          MakeShareable(new FJsonValueObject(Msgs))
	//                           });
	JsonObject->SetNumberField(TEXT("max_tokens"), 100);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	HttpRequest->SetContentAsString(RequestBody);

	// Set up the response callback
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnAPICallback);
	HttpRequest->ProcessRequest();
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
			// const TArray<TSharedPtr<FJsonValue>>* Choices;
			// if (JsonResponse->TryGetArrayField(TEXT("choices"), Choices) && Choices->Num() > 0)
			// {
				// const TSharedPtr<FJsonObject>* Message;
				// if (JsonResponse->TryGetStringField(TEXT("content"), Message))
				// {
					FString Content;
					if ((JsonResponse->TryGetStringField(TEXT("content"), Content)))
					{
						UE_LOG(LogTemp, Log, TEXT("ChatGPT Response: %s"), *Content);
					}
				// }
			// }
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get a response from ChatGPT API"));
	}
}


#undef LOCTEXT_NAMESPACE
