// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ThinkGraphEdge.generated.h"
class UThinkGraphNode;
/**
 * 
 */
UCLASS()
class THINKGRAPH_API UThinkGraphEdge : public UObject
{
	GENERATED_BODY()
public:

	/** Start or parent node for this edge */
	UPROPERTY(BlueprintReadOnly, Category = "ThinkGraph")
	UThinkGraphNode* StartNode;

	/** End or child node for this edge */
	UPROPERTY(BlueprintReadOnly, Category = "ThinkGraph")
	UThinkGraphNode* EndNode;

	/** Updates ref to OwningActor to send back event when receiving input */
	void SetOwningActor(AActor* InActor) { OwningActor = InActor; }


	
#if WITH_EDITORONLY_DATA
	// UPROPERTY(EditDefaultsOnly, Category = "ThinkGraph")
	FText NodeTitle;

	UPROPERTY(EditDefaultsOnly, Category = "Think Graph | Icons")
	FLinearColor EdgeColour = FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);
#endif

#if WITH_EDITOR
	virtual FText GetNodeTitle() const { return NodeTitle; }
	FLinearColor GetEdgeColour() const { return EdgeColour; }
	virtual void SetNodeTitle(const FText& InTitle);
#endif

protected:
	UPROPERTY()
	AActor* OwningActor;
};
