// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ThinkGraphNode.generated.h"

// class UThinkGraphEdge;
/**
 * 
 */
UCLASS()
class THINKGRAPH_API UThinkGraphNode : public UObject
{
	GENERATED_BODY()

public:
	// TODO ~Tim: Soft pointers for anim assets and other relevant assets (particles / sounds ?)

	// TODO ~Tim: Rework debug system to use BP debugger
	
	


	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UThinkGraphNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	TArray<UThinkGraphNode*> ParentNodes;

	// UPROPERTY(BlueprintReadOnly, Category = "Think Graph")
	// TMap<UThinkGraphNode*, UThinkGraphEdge*> Edges;
	
	


	// UFUNCTION(BlueprintCallable, Category = "ThinkGraph")
	// virtual UThinkGraphEdge* GetEdge(UThinkGraphNode* ChildNode);

	UFUNCTION(BlueprintCallable, Category = "ThinkGraph")
	virtual bool IsLeafNode() const;

	/** Checks all child nodes and the edge they're connected with and return the */
	// virtual UThinkGraphEdge* GetEdgeWithInput(UInputAction* InputAction);

	/** Returns whether the class is not a direct descendant of native node classes, indicating it's implemented in BP */
	bool IsHBActionSubclassedInBlueprint() const;
	

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
	
#endif

#if WITH_EDITOR
	virtual bool IsNameEditable() const;
	virtual FText GetNodeTitle() const;
	virtual void SetNodeTitle(const FText& NewTitle);


	virtual FLinearColor GetBackgroundColor() const { return FLinearColor::White; };

	virtual bool CanCreateConnection(UThinkGraphNode* Other, FText& ErrorMessage);
	virtual bool CanCreateConnectionTo(UThinkGraphNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UThinkGraphNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);
#endif
};
