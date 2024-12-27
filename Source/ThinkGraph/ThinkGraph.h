// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ThinkGraph.generated.h"

class UThinkGraphNode;

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
	
	void Tick(float DeltaTime);

	/** Represents the top level entry into various action chains*/
	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TMap<FString,float> wActionSpace;

	/** Holds all HBActionnodes defined for this graph */
	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UThinkGraphNode*> AllNodes;

#if WITH_EDITORONLY_DATA
	/** Internal Node Class type used to know which classes to consider when drawing out context menu in graphs */
	TSubclassOf<UThinkGraphNode> NodeType;

	// /** Not used currently, but might be if we allow customization of edge classes */
	// TSubclassOf<UThinkGraphEdge> EdgeType;

	UPROPERTY()
	UEdGraph* EditorGraph;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Think Graph Defaults")
	FString GraphOutputBaseDir = "ThinkGraphAssets";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Think Graph Defaults")
	FString GraphOutputPrefix;
	
	FString FinalPrompt = TEXT("I'm thinking");

private:
	/** The default skeletal mesh to use when previewing this asset */
	UPROPERTY(duplicatetransient, AssetRegistrySearchable)
	TSoftObjectPtr<class USkeletalMesh> PreviewSkeletalMesh;
#endif
};
