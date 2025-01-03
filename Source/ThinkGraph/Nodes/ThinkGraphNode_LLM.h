//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "TGNode.h"
#include "../TGTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "ThinkGraphNode_LLM.generated.h"

USTRUCT(BlueprintType)
struct FLanguageModelAPISettings
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString APIKey = TEXT(
		"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");
	// const FString APIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString APIEndpoint = TEXT("http://127.0.0.1:8080/completion");
};


struct FDataBuffer;

/**
 *  stores config for LLM usage and calls api or local model depending on config
 *	outputs prompt in a language buffer
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_LLM : public UTGNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_LLM();

	UFUNCTION(BlueprintCallable)
	virtual void Activate(UThinkGraph* ThinkGraph) override;

	void AddPromptInput(const FString Role);

	UPROPERTY(EditDefaultsOnly, Category="Model|Generate")
	uint32 NumTokensToGenerate = 100;

	UPROPERTY(EditDefaultsOnly, Category="Model|Generate", meta=(ClampMin=0, ClampMax=1))
	float Temperature = .1f;

	UPROPERTY(EditDefaultsOnly, Category="Model|Settings")
	FLanguageModelAPISettings Settings;


	UPROPERTY()
	TArray<FString> InputRoles;

protected:
	//
	void OnAPICallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	UThinkGraph* Graph;
};
