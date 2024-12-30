//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphNode.h"
#include "Interfaces/IHttpRequest.h"
#include "ThinkGraphNode_LLM.generated.h"

USTRUCT(BlueprintType)
struct FLanguageModelAPISettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString APIKey = TEXT("");
	// const FString APIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString APIEndpoint = TEXT("http://127.0.0.1:8080/completion");
};

/**
 *  stores config for LLM usage and calls api or local model depending on config
 *	outputs prompt in a language buffer
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_LLM : public UThinkGraphNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_LLM();

	UFUNCTION(BlueprintCallable)
	virtual void Activate();


	UPROPERTY(EditDefaultsOnly, Category="Model|Settings")
	FLanguageModelAPISettings Settings;

protected:
	
	void OnAPICallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
};
