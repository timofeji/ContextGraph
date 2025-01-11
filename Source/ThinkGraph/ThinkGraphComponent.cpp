// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinkGraphComponent.h"

#include "TGTypes.h"
#include "ThinkGraph.h"


// Sets default values for this component's properties
UThinkGraphComponent::UThinkGraphComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UThinkGraphComponent::InvokeStimulus(FName StimulusName, FText InText)
{
	if (Graph && Graph->InBuffers.Contains(StimulusName))
	{
		const uint16 StimulusBufferID = Graph->InBuffers[StimulusName];
		FDataBuffer& StimulusBuffer = Graph->GetBuffer(StimulusBufferID);
		StimulusBuffer.Update(InText);

		Graph->RequestBufferUpdate(StimulusBufferID);
	}
}

void UThinkGraphComponent::BindMemoryUpdateEvent(FName MemoryName, UObject* InObj, FName FuncName)
{
	if (Graph && Graph->OutBuffers.Contains(MemoryName))
	{
		FDataBuffer& MemBuffer = Graph->GetBuffer(Graph->OutBuffers[MemoryName]);
		MemBuffer.OnUpdate.AddUFunction(InObj, FuncName);
	}
}

FText UThinkGraphComponent::GetMemoryText(FName MemoryName)
{
	if (Graph && Graph->OutBuffers.Contains(MemoryName))
	{
		FDataBuffer& Out = Graph->GetBuffer(Graph->OutBuffers[MemoryName]);
		return Out.Text;
	}

	return FText();
}


// Called when the game starts
void UThinkGraphComponent::BeginPlay()
{
	Super::BeginPlay();
	Graph.LoadSynchronous();

	// ...
}


// Called every frame
void UThinkGraphComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
