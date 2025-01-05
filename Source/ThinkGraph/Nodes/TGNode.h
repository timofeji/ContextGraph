// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TGNode.generated.h"

// class UThinkGraphEdge;
class UThinkGraph;
/**
 * 
 */
UCLASS()
class THINKGRAPH_API UTGNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UTGNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UTGNode*> ParentNodes;
	
	TArray<uint16> InBufferIDS;
	TArray<uint16> OutBufferIDS;


	/** Checks all child nodes and the edge they're connected with and return the */
	// virtual UThinkGraphEdge* GetEdgeWithInput(UInputAction* InputAction);

	/** Returns whether the class is not a direct descendant of native node classes, indicating it's implemented in BP */
	bool IsHBActionSubclassedInBlueprint() const;
	
	virtual void Activate(UThinkGraph* ThinkGraph){};


#if WITH_EDITORONLY_DATA
	/**
	 * ContextMenuName is used in ThinkGraph to generate context menu items (upon right click in the graph to add new nodes)
	 *
	 * Split up ContextMenuName by "|" to create a top category if there is more than one level.
	 *
	 * You can leave this empty to exclude this class from being considered when ThinkGraph generates a context menu.
	 */
	// UPROPERTY(EditDefaultsOnly, Category = "ThinkGraph", meta = (HideInHBActioGraphDetails))
	FText ContextMenuName;
	
	// /** Set it to false to prevent context menu in graph to include the BP Class name */
	// UPROPERTY(EditDefaultsOnly, Category = "ThinkGraph", meta = (HideInHBActioGraphDetailsPanel))
	bool bIncludeClassNameInContextMenu = true;

	// UPROPERTY(EditDefaultsOnly, Category = "ThinkGraph" , meta = (ClampMin = "0", EditCondition = "ChildrenLimitType == ENodeLimit::Limited", EditConditionHides))
	int32 ChildrenLimit = 0;

	UPROPERTY()
	FText NodeTitle;
	
	UPROPERTY()
	bool bIsGenerating;
	
#endif

#if WITH_EDITOR
	virtual bool IsNameEditable() const;
	virtual FText GetNodeTitle() const;
	virtual void SetNodeTitle(const FText& NewTitle);


	virtual FLinearColor GetBackgroundColor() const { return FLinearColor::White; };

	virtual bool CanCreateConnection(UTGNode* Other, FText& ErrorMessage);
	virtual bool CanCreateConnectionTo(UTGNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UTGNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);


#endif

	
};
