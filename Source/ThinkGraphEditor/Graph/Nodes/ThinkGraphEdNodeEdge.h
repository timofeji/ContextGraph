// Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphEdNode.h"
#include "ThinkGraphEdNodeEdge.generated.h"

class UThinkGraph;
class UThinkGraphEdNodeSelector;
class UThinkGraphEdge;
class UThinkGraphEdNode;

UCLASS(MinimalAPI)
class UThinkGraphEdNodeEdge : public UThinkGraphEdNode
{
	GENERATED_BODY()

public:
	UThinkGraphEdNodeEdge();
	virtual ~UThinkGraphEdNodeEdge();

	UPROPERTY(VisibleAnywhere, Instanced, Category = "ThinkGraph")
	UThinkGraphEdge* RuntimeEdge;

	void SetRuntimeEdge(UThinkGraphEdge* InEdge);
	void CreateConnections(const UThinkGraphEdNode* Start, const UThinkGraphEdNode* End);
	// void CreateConnections(const UThinkGraphEdNodeBase* Start, const UThinkGraphEdNodeBase* End);
	void CreateBaseConnections(const UThinkGraphEdNode* Start, const UThinkGraphEdNode* End);

	UThinkGraphEdNode* GetStartNodeAsBase() const;
	UThinkGraphEdNode* GetStartNodeAsGraphNode() const;
	UThinkGraphEdNodeSelector* GetStartNodeAsSelector() const;
	UThinkGraphEdNode* GetEndNode() const;

	//~ UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual void PrepareForCopying() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UEdGraphNode interface

	void UpdateCachedIcon();
	UTexture2D* GetCachedIconTexture() const;
private:
	TSoftObjectPtr<UTexture2D> CachedIcon;

	void OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event);
};
