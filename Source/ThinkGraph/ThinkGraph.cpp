// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinkGraph.h"

#include "TGTypes.h"
#include "Nodes\TGNode.h"

void UThinkGraph::ClearGraph()
{
	for (UTGNode* Node : AllNodes)
	{
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildrenNodes.Empty();
		}
	}

	AllNodes.Empty();
	DataBuffers.Empty();
}


 FDataBuffer& UThinkGraph::AddDataBuffer()
{
	FDataBuffer& NewBuffer = DataBuffers.AddDefaulted_GetRef();
	NewBuffer.BufferID = DataBuffers.Num() - 1;
	NewBuffer.Time = 0;
	return NewBuffer;
}

void UThinkGraph::RequestBufferUpdate(unsigned short BufferID)
{
	DataBuffers[BufferID].Time++;
	
	for(auto Node :DataBuffers[BufferID].NodeDependancies)
	{
		Node->Activate(this);
	}
}

FDataBuffer& UThinkGraph::GetBuffer(unsigned short BufferID)
{
	return DataBuffers[BufferID];
}
