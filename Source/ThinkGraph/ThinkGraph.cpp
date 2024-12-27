// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinkGraph.h"

#include "Nodes/ThinkGraphNode.h"

void UThinkGraph::ClearGraph()
{
	for (UThinkGraphNode* Node : AllNodes)
	{
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildrenNodes.Empty();
		}
	}

	AllNodes.Empty();
}

void UThinkGraph::Tick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 1.f, FColor::White,
	                                 *FString::Printf(
		                                 TEXT(
			                                 "Prompt(%s)"),
		                                 *FinalPrompt));
}
