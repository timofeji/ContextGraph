// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ThinkGraph.generated.h"

struct FDataBuffer;
class UTGNode;
/**
 * 
 */
UCLASS()
class THINKGRAPH_API UThinkGraph : public UObject
{
	GENERATED_BODY()

public:
	/**
    	 * Clear all nodes in the graph 
    	 */
	void ClearGraph();

	FDataBuffer& AddDataBuffer();

	void RequestBufferUpdate(unsigned short BufferID);

	FDataBuffer& GetBuffer(unsigned short Element);

	/** @TODO ~TIM: move to editor only? **/
	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UTGNode*> AllNodes;

	TArray<FDataBuffer> DataBuffers;
	
	TMap<FName, UTGNode*> OutNodes;

#if WITH_EDITORONLY_DATA
	/** Internal Node Class type used to know which classes to consider when drawing out context menu in graphs */
	TSubclassOf<UTGNode> NodeType;

	UPROPERTY()
	UEdGraph* EditorGraph;
#endif
};
